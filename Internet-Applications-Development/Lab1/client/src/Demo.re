open Webapi.Dom;

[@bs.send] external onEvent : (Dom.document, string, Dom.event => unit) => unit
  = "addEventListener";

onEvent(document, "DOMContentLoaded", (_) => {
  Js.log("DOMContentLoaded");
});
