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

let init = () => Page.setupElementById("js-point-in-polygon-form", (form) => {
  Page.onEvent(~container=form, "button[type=submit]", "click", (e) => {
    Event.preventDefault(e);

    let _ = Js.Promise.(
      Fetch.fetchWithInit(
        Page.formAction(form) ++ "?" ++ formDataAsUrlParams(Page.formData(form)),
        Fetch.RequestInit.make(~method_=Get, ())
      )
      |> then_(Fetch.Response.text)
      |> then_((t) => {
          "js-point-in-polygon-result"
          |> Page.elementById
          |> Element.setInnerHTML(_, t)
          |> resolve;
        }));
  });
});
