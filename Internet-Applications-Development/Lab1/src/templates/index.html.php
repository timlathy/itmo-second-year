<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <title>Lab1</title>
    <script src="/client/src/Demo.bs.js" type="module"></script>
  </head>
  <body>
    <h1>Hey.</h1>
    <form>
      <fieldset>
        <legend>Line #1</legend>
        <section>
          <label for="lines0x1">From X = </label>
          <input type="text" id="lines0x1" name="lines[0][x1]">
        </section>
        <section>
          <label for="lines0y1">From Y = </label>
          <input type="text" id="lines0y1" name="lines[0][y1]">
        </section>
        <section>
          <input type="radio" id="lines0h" name="lines[0][type]" value="h" checked>
          <label for="lines0h">Horizontal</label>
          <input type="radio" id="lines0v" name="lines[0][type]" value="v">
          <label for="lines0v">Vertical</label>
          <input type="radio" id="lines0l" name="lines[0][type]" value="l">
          <label for="lines0l">Slanting</label>
          <input type="radio" id="lines0c" name="lines[0][type]" value="c">
          <label for="lines0c">Curved</label>
        </section>
        <section>
          <label for="lines0x2">To X = </label>
          <input type="text" id="lines0x2" name="lines[0][x2]">
        </section>
        <section>
          <label for="lines0y2">To Y = </label>
          <input type="text" id="lines0y2" name="lines[0][y2]">
        </section>
        <section>
          <label for="lines0y2">Curve X = </label>
          <input type="text" id="lines0x3" name="lines[0][x3]">
        </section>
        <section>
          <label for="lines0y2">Curve Y = </label>
          <input type="text" id="lines0y3" name="lines[0][y3]">
        </section>
      </fieldset>
    </form>
  </body>
</html>

