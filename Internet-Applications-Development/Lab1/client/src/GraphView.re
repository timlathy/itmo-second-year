open Webapi.Dom;

let graphUrl = (g: GraphStorage.graph): string => {
  let encoded = g |> GraphStorage.graphAsJson |> Js.Global.encodeURIComponent;
  {j|/graphs/show?g=$encoded|j}
};

let formDataAsUrlParams: (Page.formData) => string = [%bs.raw {|
  function (formData) {
    var params = new URLSearchParams();
    for (var e of formData) params.append(e[0], e[1]);
    return params.toString();
  }
|}];

let fetchPipResult = (form: Dom.element): unit => {
  let _ = Js.Promise.(
    Fetch.fetchWithInit(
      Page.formAction(form) ++ "?" ++ formDataAsUrlParams(Page.formData(form)),
      Fetch.RequestInit.make(~method_=Get, ())
    )
    |> then_(Fetch.Response.text)
    |> then_((result) => {
        "js-pip-history"
        |> Page.elementById
        |> Element.insertAdjacentHTML(BeforeEnd, result)
        |> resolve;
      }));
}

let init = () => Page.setupElementById("js-pip-form", (form) => {
  Page.overrideClick(~container=form, "button[type=submit]",
                     () => fetchPipResult(form));
  Page.overrideClick("#js-pip-history-clear", () => {
    "js-pip-history"
    |> Page.elementById
    |> Element.setInnerHTML(_, "");
  });

  let graphName: string =
    "js-pip-graph-name" |> Page.elementById |> Element.textContent;

  "js-pip-preview"
  |> Page.elementById
  |> Element.setInnerHTML(_, GraphStorage.loadPreviewByName(graphName));
});
