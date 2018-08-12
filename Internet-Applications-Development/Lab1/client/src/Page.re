open Webapi.Dom;

[@bs.val] external doc : Dom.element = "document";

[@bs.send.pipe : Dom.document] external unsafeGetElementById :
  (string) => Dom.element = "getElementById";

[@bs.send.pipe : Dom.element] external unsafeQuerySelector :
  (string) => Dom.element = "querySelector";

let elementById = unsafeGetElementById(_, document);

/* Classes */

let elementClassList = (selector: string, parent: Dom.element) =>
  parent |> unsafeQuerySelector(selector) |> Element.classList;

let hide = (selector: string, parent: Dom.element) =>
  parent |> elementClassList(selector) |> DomTokenList.add("hidden");

let unhide = (selector: string, parent: Dom.element) =>
  parent |> elementClassList(selector) |> DomTokenList.remove("hidden");

/* Events */

[@bs.send] external onDocumentEvent :
  (Dom.document, string, Dom.event => unit) => unit = "addEventListener";

let onEvent = (~container: Dom.element=doc, selector: string,
               event: string, handler: Dom.event => unit): unit => {
  container
  |> unsafeQuerySelector(selector)
  |> Element.asEventTarget
  |> EventTarget.addEventListener(event, handler);
};

let onDomContentLoaded = onDocumentEvent(document, "DOMContentLoaded");

/* Forms */

type formData;

[@bs.new] external formData : Dom.element => formData = "FormData";

[@bs.send.pipe : formData] external forEachFormInput
  : (string => string => unit) => unit = "forEach";
