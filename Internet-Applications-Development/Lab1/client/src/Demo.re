open Webapi.Dom;

[@bs.send] external onEvent : (Dom.document, string, Dom.event => unit) => unit
  = "addEventListener";

let lineInputSectionHtml = (num: int, prop: string, label: string,
                            ~hidden: bool): string =>
  "<section " ++ (hidden ? "class=\"hidden\">" : ">") ++
  {j|<label for="lines$(num)_$prop">$label</label>|j} ++ 
  {j|<input type="text" id="lines$num$prop" name="lines[$num][$prop]">|j} ++
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
  lineInputSectionHtml(num, "x1", "From X = ", ~hidden=false) ++
  lineInputSectionHtml(num, "y1", "From Y = ", ~hidden=false) ++
  "<section>" ++
  lineInputRadioHtml(num, "type", "h", "Horizontal") ++
  lineInputRadioHtml(num, "type", "v", "Vertical") ++
  lineInputRadioHtml(num, "type", "l", "Slanting") ++
  lineInputRadioHtml(num, "type", "q", "Curved") ++
  "</section>" ++
  lineInputSectionHtml(num, "x2", "To X = ", ~hidden=true) ++
  lineInputSectionHtml(num, "y2", "To Y = ", ~hidden=true) ++
  lineInputSectionHtml(num, "x3", "Curve X = ", ~hidden=true) ++
  lineInputSectionHtml(num, "y3", "Curve Y = ", ~hidden=true) ++
  "</fieldset>";
};

onEvent(document, "DOMContentLoaded", (_) => {
  document
    |> Document.getElementById("graph-lines")
    |> Belt.Option.getExn
    |> Element.setInnerHTML(_, lineFieldsetHtml(0));
});
