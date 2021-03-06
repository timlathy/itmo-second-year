<?php include __DIR__ . '/../_head.html.php'; ?>

<form id="js-graph-form" class="graph-form">
  <input type="text" class="title-input" name="graph-name" placeholder="Graph name" />
  <div class="column-layout">
    <div class="column-layout__fixed">
      <h2>Lines<button id="js-graph-form-new-line" class="button button--inline button--plus"></button></h2>
      <div id="js-graph-form-line-container"></div>
    </div>
    <div class="column-layout__gap"></div>
    <div class="column-layout__flexible">
      <h2>Graph<button id="js-graph-form-preview" class="button button--inline button--refresh"></button></h2>
      <section class="card card--error hidden" id="js-error-container"></section>
      <section id="js-graph-form-help" class="card card--info">
        <p>
          The graph has two axes, X and Y, extending from -50 to 50. Coordinates are multiplied by 10, so you can
          effectively the area ranging from -5 to 5, with a floating point precision of 0.1.
        </p>
        <p>          
          Use arithmetic expressions like <em>5 * (3 + R)</em> to define the lines making up a polygon.
        </p>
        <p>
          One or more letters represent a variable,
          which are specified at computation time; you also need to provide dummy values to render a preview.
        </p>
        <p>
          <em>R</em> (stands for <em>radius</em>) is a special variable that is restricted to a [1; 4] range.
        </p>
        <a id="js-graph-form-help-toggle" class="card__link card__link--info">Hide this message</a>
      </section>
      <div class="row-layout">
        <section class="graph-form__preview row-layout__item" id="js-graph-form-preview-container"></section>
        <section class="row-layout__item">
          <label class="input-label" for="line-input-variables">Preview variables</label>
          <input class="input" type="text" id="line-input-variables" name="variables" placeholder="R = 2.3, S = ..." />
          <button id="js-graph-form-save" class="button button--primary" type="submit">Save</button>
        </section>
      </div>
    </div>
  </div>
</form>

<?php include __DIR__ . '/../_footer.html.php'; ?>
