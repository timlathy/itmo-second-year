open Webapi.Dom;

[@bs.val] external doc : Dom.element = "document";

[@bs.send.pipe : Dom.document] external unsafeGetElementById :
  (string) => Dom.element = "getElementById";

[@bs.send.pipe : Dom.element] external querySel :
  (string) => Dom.element = "querySelector";

[@bs.get] external formAction : (Dom.element) => string = "action";

[@bs.get] external inputValue : (Dom.element) => string = "value";

[@bs.get] external isChecked : (Dom.element) => bool = "checked";

[@bs.set] external setInputValueFlip : (Dom.element, string) => unit = "value";

[@bs.val] external nodeListToArray : Dom.nodeList => array(Dom.element) =
  "Array.prototype.slice.call";

let setInputValue = (value: string, element: Dom.element): unit =>
  setInputValueFlip(element, value);

let elementById = unsafeGetElementById(_, document);

let setupElementById = (id: string, setupFn: (Dom.element => unit)): unit =>
  switch (Document.getElementById(id, document)) {
  | Some(element) => setupFn(element)
  | _ => ();
  }

/* Classes */

let elementClassList = (selector: string, parent: Dom.element) =>
  parent |> querySel(selector) |> Element.classList;

let hide = (selector: string, parent: Dom.element) =>
  parent |> elementClassList(selector) |> DomTokenList.add("hidden");

let unhide = (selector: string, parent: Dom.element) =>
  parent |> elementClassList(selector) |> DomTokenList.remove("hidden");

let hideAll = (selectors: array(string), parent: Dom.element): unit =>
  selectors |> Js.Array.forEach(hide(_, parent));

let unhideAll = (selectors: array(string), parent: Dom.element): unit =>
  selectors |> Js.Array.forEach(unhide(_, parent));

/* Events */

[@bs.send] external onDocumentEvent :
  (Dom.document, string, Dom.event => unit) => unit = "addEventListener";

let onEvent = (~container: Dom.element=doc, selector: string,
               event: string, handler: Dom.event => unit): unit => {
  container
  |> querySel(selector)
  |> Element.asEventTarget
  |> EventTarget.addEventListener(event, handler);
};

let overrideClick = (~container: Dom.element=doc, selector: string,
                     handler: (unit) => unit): unit =>
  onEvent(~container, selector, "click", (e) => {
    Event.preventDefault(e);
    handler();
  });

/* Forms */

type formData;

[@bs.new] external formData : Dom.element => formData = "FormData";

[@bs.new] external formDataBody : Dom.element => Fetch.bodyInit = "FormData";
