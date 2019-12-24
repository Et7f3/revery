let menuSupported: unit => bool;

type menu;

let make:
  (~key: Brisk_reconciler.Key.t=?, unit) => Brisk_reconciler.element(menu);
