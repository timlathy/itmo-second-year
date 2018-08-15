open Webapi.Dom;

[@bs.send.pipe : string] external stringSlice : (int) => string = "slice";

[@bs.send.pipe : Dom.element] external unsafeClosest :
  (string) => Dom.element = "closest";

let lineInputSectionHtml = (num: int, ~cls: option(string)=?,
                            prop: string, label: string): string =>
  "<section" ++ (switch(cls) {
  | Some(className) => " class=\"" ++ className ++ "\">"
  | None => ">"
  }) ++
  {j|<label for="lines$(num)_$prop">$label</label>|j} ++ 
  {j|<input type="text" class="js-line-input" name="lines[$num][$prop]">|j} ++
  "</section>";

let lineInputRadioHtml = (num: int, prop: string, value: string,
                          label: string): string => {
  let inputId = {j|lines$(num)_$(prop)_$value|j};
  let inputName = {j|lines[$num][$prop]|j};
  {j|<input type="radio" id="$inputId" name="$inputName" value="$value">|j} ++
  {j|<label for="$inputId">$label</label>|j}
};

let lineFieldsetHtml = (num: int): string => {
  let displayNum = num + 1;
  {j|<fieldset id="lines$num"><legend>Line #$displayNum</legend>|j} ++
  {j|<input type="hidden" class="js-line-input js-line-input-type" />|j} ++
  lineInputSectionHtml(num, "x1", "From X = ") ++
  lineInputSectionHtml(num, "y1", "From Y = ") ++
  "<section>" ++
  lineInputRadioHtml(num, "type", "h", "Horizontal") ++
  lineInputRadioHtml(num, "type", "v", "Vertical") ++
  lineInputRadioHtml(num, "type", "l", "Slanting") ++
  lineInputRadioHtml(num, "type", "q", "Curved") ++
  "</section>" ++
  lineInputSectionHtml(num, ~cls="js-line-x2 hidden", "x2", "To X = ") ++
  lineInputSectionHtml(num, ~cls="js-line-y2 hidden", "y2", "To Y = ") ++
  lineInputSectionHtml(num, ~cls="js-line-x3 hidden", "x3", "Curve X = ") ++
  lineInputSectionHtml(num, ~cls="js-line-y3 hidden", "y3", "Curve Y = ") ++
  "</fieldset>";
};

let lineFieldsetClick = (e: Dom.event): unit => {
  let target = e |> Event.target |> EventTarget.unsafeAsElement;
  let fields = target |> unsafeClosest("fieldset");

  target
  |> Element.id
  |> stringSlice(-6)
  |> fun
    | "type_h" => {
      fields |> Page.unhide(".js-line-x2");
      fields |> Page.hideAll([|".js-line-y2", ".js-line-x3", ".js-line-y3"|]);
      fields |> Page.querySel(".js-line-input-type") |> Page.setInputValue("h");
    }
    | "type_v" => {
      fields |> Page.unhide(".js-line-y2");
      fields |> Page.hideAll([|".js-line-x2", ".js-line-x3", ".js-line-y3"|]);
      fields |> Page.querySel(".js-line-input-type") |> Page.setInputValue("v");
    }
    | "type_l" => {
      fields |> Page.unhideAll([|".js-line-x2", ".js-line-y2"|]);
      fields |> Page.hideAll([|".js-line-x3", ".js-line-y3"|]);
      fields |> Page.querySel(".js-line-input-type") |> Page.setInputValue("l");
    }
    | "type_q" => {
      fields |> Page.unhideAll([|".js-line-x2", ".js-line-y2",
                                 ".js-line-x3", ".js-line-y3"|]);
      fields |> Page.querySel(".js-line-input-type") |> Page.setInputValue("q");
    }
    | _ => ();
};

let setupLineFieldsetEvents = (num: int): unit => {
  Page.onEvent({j|#lines$num|j}, "click", lineFieldsetClick);
};

let init = () => Page.setupElementById("line-input-form", (form) => {
  "line-input-container"
  |> Page.elementById
  |> Element.setInnerHTML(_, lineFieldsetHtml(0) ++ lineFieldsetHtml(1));

  Page.onEvent("#line-input-preview", "click", (e) => {
     Event.preventDefault(e);

     let _ = Js.Promise.(
       Fetch.fetchWithInit("/graphs/preview", Fetch.RequestInit.make(
          ~method_=Post, ~body=Page.formDataBody(form), ()))
       |> then_(Fetch.Response.text)
       |> then_((t) => {
           "line-input-preview-container"
           |> Page.elementById
           |> Element.setInnerHTML(_, t)
           |> resolve;
         }));
   });

  Page.onEvent("#line-input-save", "click", (e) => {
    Event.preventDefault(e);

    let name = form |> Page.querySel("[name=graph-name]") |> Page.inputValue;
    let lines: array(GraphStorage.graphLine) =
      document
      |> Document.querySelectorAll(".js-line-input")
      |> Page.nodeListToArray
      |> Js.Array.map(Page.inputValue)
      |> Utils.chunkArray(7)
      |> Js.Array.map(fun
        | [|type_, x1, y1, x2, y2, x3, y3|] =>
          GraphStorage.{type_, x1, y1, x2, y2, x3, y3}
        | _ =>
          Js.Exn.raiseError("Malformed input form"));

    GraphStorage.append(GraphStorage.{name, lines});
  });

  setupLineFieldsetEvents(0);
});
