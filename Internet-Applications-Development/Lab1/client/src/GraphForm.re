open Webapi.Dom;

[@bs.send.pipe : string] external stringSlice : (int) => string = "slice";

[@bs.send.pipe : Dom.element] external unsafeClosest :
  (string) => Dom.element = "closest";

/* Depends on the number of .color-swatch--n CSS definitions */
let maxLines = 8;

let lineInputSectionHtml = (num: int, ~cls: option(string)=?,
                            prop: string, label: string): string => {
  let sectionClass = switch (cls) {
  | Some(className) => " class=\"" ++ className ++ "\""
  | _ => ""
  };
  let inputId = {j|lines$(num)_$prop|j};
  let inputName = {j|lines[$num][$prop]|j};
  {j|<section$sectionClass>|j} ++
  {j|<label class="input-label" for="$inputId">$label</label>|j} ++ 
  {j|<input type="text" id="$inputId" class="input js-line-input" name="$inputName">|j} ++
  "</section>"
}

let lineInputRadioHtml = (num: int, prop: string, value: string,
                          label: string): string => {
  let inputId = {j|lines$(num)_$(prop)_$value|j};
  let inputName = {j|lines[$num][$prop]|j};
  {j|<input class="radio-switch__input" type="radio" id="$inputId" name="$inputName" value="$value">|j} ++
  {j|<label class="radio-switch__label" for="$inputId">$label</label>|j}
};

let lineFieldsetHtml = (num: int): string => {
  let displayNum = num + 1;
  {j|<fieldset id="js-line-fieldset-$num" class="card"><div class="card__title">|j} ++
  {j|<div class="color-swatch color-swatch--$num"></div>Line #$displayNum</div>|j} ++
  {j|<section class="radio-switch">|j} ++
  lineInputRadioHtml(num, "type", "h", "Horizontal") ++
  lineInputRadioHtml(num, "type", "v", "Vertical") ++
  lineInputRadioHtml(num, "type", "l", "Slanting") ++
  lineInputRadioHtml(num, "type", "q", "Curved") ++
  "</section>" ++
  {j|<input type="hidden" class="js-line-input js-line-input-type" />|j} ++
  lineInputSectionHtml(num, "x1", "X1 (starting point): ") ++
  lineInputSectionHtml(num, "y1", "Y1 (starting point): ") ++
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

let insertNewLine = (): unit => {
  let container = Page.elementById("js-graph-form-line-container");
  let lineNo = Element.childElementCount(container);

  if (lineNo < maxLines) {
    Element.insertAdjacentHTML(BeforeEnd, lineFieldsetHtml(lineNo), container);
    Page.onEvent({j|#js-line-fieldset-$lineNo|j}, "click", lineFieldsetClick);
  }
};

let fetchAndDisplayPreview = (): Js.Promise.t(option(string)) => {
  let form = Page.elementById("js-graph-form");
  Js.Promise.(
    Fetch.fetchWithInit("/graphs/preview", Fetch.RequestInit.make(
       ~method_=Post, ~body=Page.formDataBody(form), ()))
    |> then_(Error.checkResponse)
    |> then_(Fetch.Response.text)
    |> then_((t) => {
        Error.hide();

        "js-graph-form-preview-container"
        |> Page.elementById
        |> Element.setInnerHTML(_, t);

        resolve(Some(t));
    })
    |> catch((e) => {
        e |> Error.lineErrorFromPromise |> Error.lineErrorHtml |> Error.show;

        resolve(None : option(string));
    }))
};

let saveGraph = (): unit => {
  let name = Page.doc |> Page.querySel("[name=graph-name]") |> Page.inputValue;

  if (GraphStorage.nameExists(name)) {
    Error.show({j|<strong>An error has occurred:</strong> The name "$name" is already taken|j});
  }
  else {
    fetchAndDisplayPreview()
    |> Js.Promise.then_(fun
        | Some(previewSvg) => {
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
          let graph = GraphStorage.{name, lines};

          GraphStorage.append(graph);
          GraphStorage.setPreviewByName(name, previewSvg);
          Window.setLocation(window, GraphView.graphUrl(graph));
          Js.Promise.resolve(());
        }
        | _ => Js.Promise.resolve(())
    )
    |> ignore;
  }
};

let init = () => Page.setupElementById("js-graph-form", (_) => {
  insertNewLine();

  Page.overrideClick("#js-graph-form-new-line", insertNewLine);
  Page.overrideClick("#js-graph-form-preview", () => ignore(fetchAndDisplayPreview()));
  Page.overrideClick("#js-graph-form-save", saveGraph);
  Page.overrideClick("#js-graph-form-help-toggle", () => {
    "js-graph-form-help"
    |> Page.elementById
    |> Element.classList
    |> DomTokenList.add("hidden");
  });
});
