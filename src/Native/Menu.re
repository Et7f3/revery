open Brisk_reconciler;

[@noalloc] external menuSupported: unit => bool = "revery_menuSupported";

type menu;

external createMenu: unit => menu = "revery_create_menu";

let%nativeComponent make = ((), hooks) => (
  {
    make: createMenu,
    configureInstance: (~isFirstRender, obj) => obj,
    children: Brisk_reconciler.empty,
    insertNode: (~parent, ~child: _, ~position: _) => parent,
    deleteNode: (~parent, ~child: _, ~position: _) => parent,
    moveNode: (~parent, ~child: _, ~from: _, ~to_: _) => parent,
  },
  hooks,
);
