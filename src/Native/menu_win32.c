#ifdef WIN32

#include <stdio.h>
#include <stdbool.h>

#include <Windows.h>
#include <winuser.h>

#include <caml/alloc.h>
#include <caml/callback.h>
#include <caml/custom.h>
#include <caml/memory.h>

#define WM_POPUP WM_USER + 14
/*
** We need to have a unique number to avoid clash with other message
** of GLFW for instance.
**
** So I have pick the line number as random one.
** TODO: check that this number isn't used in our dependencies
*/

/*
** Define custom operations here
*/

struct s_menu
{
    void * menu_handle;
};

#define Menu_val(v) (*((struct s_menu *)Data_custom_val(v)))

CAMLprim int menu_compare(value v1, value v2)
{
  return Val_int(Menu_val(v1).menu_handle - Menu_val(v2).menu_handle);
}

/*
** Encapsulation of opaque menu handles (of type s_menu)
** as OCaml custom blocks.
*/

static struct custom_operations menu_ops = {
    "com.outrunlabs.www.revery.menu",
    custom_finalize_default,
    menu_compare,
    custom_hash_default,
/*
** ASK: is it useful to define custom_hash_default
*/
    custom_serialize_default,
    custom_deserialize_default,
    custom_compare_ext_default
};

static value * g_menu_dispatch = NULL;
static value * g_popup_dispatch = NULL;
static value * g_menu_list = NULL;
static void * g_hook_handle = NULL;

LRESULT CALLBACK WndProc(int msg, WPARAM wParam, LPARAM lParam)
{
    CAMLparam0();
    MSG m;

    if (msg == HC_ACTION && wParam == PM_REMOVE)
    {
        m = *(MSG *)lParam;
        if (m.message == WM_COMMAND)
        {
            printf("%d is clicked\n", LOWORD(m.wParam));

            /* First time around, look up by name */
            if (g_menu_dispatch == NULL)
                g_menu_dispatch = caml_named_value("menu_dispatch");
            caml_callback(*g_menu_dispatch, Val_int(LOWORD(m.wParam)));
        }
        else if (m.message == WM_POPUP)
        {
            printf("%d is clicked\n", LOWORD(m.wParam));

            /* First time around, look up by name */
            if (g_popup_dispatch == NULL)
                g_popup_dispatch = caml_named_value("popup_dispatch");
            caml_callback(*g_popup_dispatch, Val_int(LOWORD(m.wParam)));
        }
    }

    CAMLreturnT(LRESULT, CallNextHookEx((void *) NULL, msg, wParam, lParam));
}

value revery_create_menu_win32(void)
{
    value ret = alloc_custom(&menu_ops, sizeof(struct s_menu), 0, 1);

    Menu_val(ret).menu_handle = CreateMenu();

    return ret;
}

value revery_create_sub_menu_win32(void)
{
    /*
      TODO: Make another structure when we will need to free ressource
    */
    value ret = alloc_custom(&menu_ops, sizeof(struct s_menu), 0, 1);

    Menu_val(ret).menu_handle = CreatePopupMenu();

    return ret;
}

void release_hook(void)
{
/*
** we are registered only if we add a hook
*/
    UnhookWindowsHookEx(g_hook_handle);
}

value revery_add_string_item_menu_win32(value vMenu, int uid, const char * pMessage)
{
    bool ret = AppendMenu(Menu_val(vMenu).menu_handle, MF_STRING, uid, pMessage);

    if (!g_hook_handle)
    {
        g_hook_handle = SetWindowsHookExW(WH_GETMESSAGE, WndProc, (HINSTANCE)NULL, GetCurrentThreadId());
        atexit(release_hook);
    }

    return ret;
}

value revery_add_separator_sub_menu_win32(value vMenu, int uid, const char * pMessage)
{
    bool ret = AppendMenu(Menu_val(vMenu).menu_handle, MF_SEPARATOR, 0, NULL);

    return ret;
}

value revery_add_sub_menu_win32(value vMenu, value vSub, const char * pMessage)
{
    bool ret = AppendMenu(Menu_val(vMenu).menu_handle, MF_POPUP, (UINT_PTR)Menu_val(vSub).menu_handle, pMessage);

    return ret;
}

value revery_assign_menu_win32(void * pWindow, value vMenu)
{
/*
** TODO: apply this remarks
** Remarks:
**
** The window is redrawn to reflect the menu change.
** A menu can be assigned to any window that is not a child window.
**
** The SetMenu function replaces the previous menu, if any,
** but it does not destroy it. An application should call the DestroyMenu
** function to accomplish this task.
*/
    return SetMenu(pWindow, Menu_val(vMenu).menu_handle);
}

value revery_popup_sub_menu_win32(void * pWindow, value vMenu, int x, int y)
{
    CAMLparam1(vMenu);
    WINDOWINFO wndi = {sizeof(WINDOWINFO)};

/*SystemParametersInfo(SPI_SETMENUANIMATION, 0, (PVOID)true, 0);*/

    if (GetWindowInfo(pWindow, &wndi))
    {
        /*
        ** We have managed to access window position
        */
        x += wndi.rcClient.left;
        y += wndi.rcClient.top;
    }

    int ret = TrackPopupMenu(Menu_val(vMenu).menu_handle,
        TPM_LEFTALIGN | TPM_TOPALIGN | TPM_NONOTIFY | TPM_RETURNCMD
      | TPM_LEFTBUTTON/* | TPM_HORPOSANIMATION | TPM_VERPOSANIMATION*/,
      x, y, 0 /* reserved */, pWindow, NULL /* ignored */
    );

    if (ret)
        PostMessage(pWindow, WM_POPUP, ret, 0);

    CAMLreturn(ret == 0);
}

value revery_get_application_menu_win32(void * pWindow, value list)
{
  CAMLparam1(list);
  CAMLlocal1(ret);

#if 0
  while (Is_block(list))
  {
    printf("the variable list is a block: %p, contain: %p\n", Field(list, 0), NULL /*Menu_val(Field(list, 0)).menu_handle*/);
    list = Field(list, 0);
  }
  ret = GetMenu(pWindow);
#endif
/*
** this function try to iterate over menu to find the right one object
*/

  CAMLreturn(ret);
}
#endif