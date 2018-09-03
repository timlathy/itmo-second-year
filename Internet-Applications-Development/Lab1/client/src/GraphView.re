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
  let Some(graphName) = Element.getAttribute("data-graph-name", form);

  let _ = Js.Promise.(
    Fetch.fetchWithInit(
      Page.formAction(form) ++ "?" ++ formDataAsUrlParams(Page.formData(form)),
      Fetch.RequestInit.make(~method_=Get, ())
    )
    |> then_(Error.checkResponse)
    |> then_(Fetch.Response.text)
    |> then_((result) => {
        Error.hide();

        GraphStorage.appendHistory(graphName, result);

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

let loadHistory = (form: Dom.element): unit => {
  let Some(graphName) = Element.getAttribute("data-graph-name", form);
  let history = Page.elementById("js-pip-history");

  graphName
  |> GraphStorage.loadHistory
  |> Js.Array.joinWith("")
  |> Element.setInnerHTML(history, _);
};

let setupInputPersistence = (form: Dom.element): unit => {
  let Some(graphName) = Element.getAttribute("data-graph-name", form);

  form
  |> Element.querySelectorAll("[data-input]")
  |> Page.nodeListToArray
  |> Js.Array.forEach((input) => {
      let Some(inputName) = Element.getAttribute("data-input", input);

      switch (GraphStorage.getInputValue(graphName, inputName)) {
      | Some(saved) => Element.setAttribute("value", saved, input)
      | _ => ()
      };

      input
      |> Element.asEventTarget
      |> EventTarget.addEventListener("change", (_) => {
          input
          |> Page.inputValue
          |> GraphStorage.setInputValue(graphName, inputName);
        });
    });

  form
  |> Element.querySelectorAll("input[type=checkbox]")
  |> Page.nodeListToArray
  |> Js.Array.forEach((checkbox) => {
      let Some(checkName) = Element.getAttribute("name", checkbox);
      
      if (GraphStorage.isInputChecked(graphName, checkName))
        Element.setAttribute("checked", "true", checkbox);

      checkbox
      |> Element.asEventTarget
      |> EventTarget.addEventListener("change", (_) => {
          checkbox |> Page.isChecked |> GraphStorage.toggleInput(graphName, checkName);
        });
    });
};

let inputValid = (form: Dom.element): bool => {
  let isValid = 
    form
    |> Element.querySelectorAll("[data-input]")
    |> Page.nodeListToArray
    |> Js.Array.reduce((pastValid, input) => {
        if (!pastValid) false
        else {
          let name = Element.getAttribute("data-input", input);
          let rawValue = input |> Page.inputValue;
          let value = Js.Float.fromString(rawValue);

          let min = switch (Element.getAttribute("min", input)) {
          | Some(v) => Js.Float.fromString(v);
          | _ => -5.0;
          }
          let max = switch (Element.getAttribute("max", input)) {
          | Some(v) => Js.Float.fromString(v);
          | _ => 5.0;
          }

          if (Js.Float.isNaN(value)) {
            Error.show({j|<strong>Please check your input data:</strong> "$rawValue" is not a valid numerical value for $name.|j});
            false
          }
          else if (value < min) {
            Error.show({j|<strong>Please check your input data:</strong> Smallest possible value for $name is $min.|j});
            false
          }
          else if (value > max) {
            Error.show({j|<strong>Please check your input data:</strong> Largest possible value for $name is $max.|j});
            false
          }
          else true
        }
      }, true);

  if (isValid) Error.hide();
  isValid
};

let init = () => Page.setupElementById("js-pip-form", (form) => {
  let Some(graphName) = Element.getAttribute("data-graph-name", form);

  Page.overrideClick(~container=form, "button[type=submit]", () => {
    if (inputValid(form)) fetchPipResult(form)
  });
  Page.overrideClick("#js-pip-history-clear", () => {
    "js-pip-history"
    |> Page.elementById
    |> Element.setInnerHTML(_, "");

    GraphStorage.clearHistory(graphName);
  });

  let graphName: string =
    "js-pip-graph-name" |> Page.elementById |> Element.textContent;

  "js-pip-preview"
  |> Page.elementById
  |> Element.setInnerHTML(_, GraphStorage.loadPreviewByName(graphName));

  setupInputPersistence(form);
  loadHistory(form);
});
