let checkResponse: (Fetch.response) => Js.Promise.t(Fetch.response) = [%bs.raw {|
  function(response) {
    return response.ok ? response : response.json().then((j) => { throw j; });
  }
|}];

[@bs.deriving abstract]
type lineError = { line: int, message: string };

external lineErrorFromPromise : Js.Promise.error => lineError = "%identity";

let lineErrorHtml = (err: lineError): string => {
  let lineNo = err->lineGet + 1;
  {j|<strong>An error has occurred on line #$lineNo:</strong> |j} ++ err->messageGet
}

let hide = (): unit => Page.hide("#js-error-container", Page.doc);

let show = (errorHtml: string): unit => {
  Page.unhide("#js-error-container", Page.doc);

  "js-error-container"
  |> Page.elementById
  |> Webapi.Dom.Element.setInnerHTML(_, errorHtml);
};
