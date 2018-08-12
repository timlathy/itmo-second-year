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
  {j|<input type="text" id="lines$(num)_$prop" name="lines[$num][$prop]">|j} ++
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
  let form = target |> unsafeClosest("form");

  let hideAll = (s, f) => s |> Js.Array.forEach(Page.hide(_, f));
  let unhideAll = (s, f) => s |> Js.Array.forEach(Page.unhide(_, f));
  
  target
  |> Element.id
  |> stringSlice(-6)
  |> fun
    | "type_h" => {
      form |> Page.unhide(".js-line-x2");
      form |> hideAll([|".js-line-y2", ".js-line-x3", ".js-line-y3"|]);
    }
    | "type_v" => {
      form |> Page.unhide(".js-line-y2");
      form |> hideAll([|".js-line-x2", ".js-line-x3", ".js-line-y3"|]);
    }
    | "type_l" => {
      form |> unhideAll([|".js-line-x2", ".js-line-y2"|]);
      form |> hideAll([|".js-line-x3", ".js-line-y3"|]);
    }
    | "type_q" =>
      form |> unhideAll([|".js-line-x2", ".js-line-y2",
                          ".js-line-x3", ".js-line-y3"|])
    | _ => ();
};

let setupLineFieldsetEvents = (num: int): unit => {
  Page.onEvent({j|#lines$num|j}, "click", lineFieldsetClick);
};

[@bs.new] external formDataBody : Dom.element => Fetch.bodyInit = "FormData";

Page.onDomContentLoaded((_) => {
  "line-input-container"
  |> Page.elementById
  |> Element.setInnerHTML(_, lineFieldsetHtml(0));

  Page.onEvent("#line-input-preview", "click", (e) => {
     Event.preventDefault(e);

     let form = Page.elementById("line-input-form");
     let _ = Js.Promise.(
       Fetch.fetchWithInit("/graphs/preview",
         Fetch.RequestInit.make(~method_=Post, ~body=formDataBody(form), ()),
       )
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

    "line-input-form"
    |> Page.elementById
    |> Page.formData
    |> Page.forEachFormInput((v, k) => {
      /* ...? */     
      ();
    });

    let lines: array(GraphStorage.graphLine) = [| |];
    let graph: GraphStorage.graph = {name: "h", lines};

    GraphStorage.save(graph);
  });

  setupLineFieldsetEvents(0);
});
