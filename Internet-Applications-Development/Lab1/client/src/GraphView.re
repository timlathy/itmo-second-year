open Webapi.Dom;

let graphUrl = (g: GraphStorage.graph): string => {
  let encoded = g |> GraphStorage.graphAsJson |> Js.Global.encodeURIComponent;
  {j|/graphs/show?g=$encoded|j}
};

let formDataAsUrlParams: (Page.formData) => string = [%bs.raw {|
  function (formData) {
    var params = new URLSearchParams();
    for (var kv of formData) { params.append(kv[0], kv[1]); };
    return params.toString();
  }
|}];

let fetchPipResult = (form: Dom.element): unit => {
  let _ = Js.Promise.(
    Fetch.fetchWithInit(
      Page.formAction(form) ++ "?" ++ formDataAsUrlParams(Page.formData(form)),
      Fetch.RequestInit.make(~method_=Get, ())
    )
    |> then_(Error.checkResponse)
    |> then_(Fetch.Response.text)
    |> then_((result) => {
        Error.hide();

        "js-pip-history"
        |> Page.elementById
        |> Element.insertAdjacentHTML(AfterBegin, result)
        |> resolve;
      })
    |> catch((e) => {
        e |> Error.lineErrorFromPromise |> Error.lineErrorHtml |> Error.show;
        resolve(());
    }));
}

let inputValid = (form: Dom.element): bool => {
  let isValid = 
    form
    |> Element.querySelectorAll("[data-pip-variable]")
    |> Page.nodeListToArray
    |> Js.Array.reduce((pastValid, input) => {
        if (!pastValid) false
        else {
          let name = Element.getAttribute("data-pip-variable", input);
          let rawValue = input |> Page.inputValue;
          let value = Js.Float.fromString(rawValue);

          let isNonNegative = name == Some("R");

          if (Js.Float.isNaN(value)) {
            Error.show({j|<strong>Please check your input data:</strong> "$rawValue" is not a valid numerical value for $name.|j});
            false
          }
          else if (isNonNegative && value < 0.0) {
            Error.show({j|<strong>Please check your input data:</strong> A non-negative value is expected for $name.|j});
            false
          }
          else true
        }
      }, true);

  if (isValid) Error.hide();
  isValid
};

let init = () => Page.setupElementById("js-pip-form", (form) => {
  Page.overrideClick(~container=form, "button[type=submit]", () => {
    if (inputValid(form)) fetchPipResult(form)
  });
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
