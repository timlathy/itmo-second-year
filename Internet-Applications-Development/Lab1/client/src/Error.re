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
