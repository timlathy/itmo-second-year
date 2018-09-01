<?php include __DIR__ . '/../_head.html.php'; ?>

<form id="js-graph-form" class="graph-form">
  <input type="text" class="title-input" name="graph-name" placeholder="New Graph" />
  <div class="column-layout">
    <div class="column-layout__fixed column-layout__fixed--narrow">
      <h2>Lines<button id="js-graph-form-new-line" class="button button--inline button--plus"></button></h2>
      <div id="js-graph-form-line-container"></div>
    </div>
    <div class="column-layout__gap"></div>
    <div class="column-layout__fixed column-layout__fixed--narrow">
      <h2>Preview<button id="js-graph-form-preview" class="button button--inline button--refresh"></button></h2>
      <section class="card card--error hidden" id="js-error-container"></section>
      <section class="graph-form__preview" id="js-graph-form-preview-container"></section>
      <label class="input-label" for="line-input-variables">Preview variables</label>
      <input class="input" type="text" id="line-input-variables" name="variables" placeholder="R = 2.3, S = ..." />
      <button id="js-graph-form-save" class="button button--primary" type="submit">Save</button>
    </div>
  </div>
</form>

<?php include __DIR__ . '/../_footer.html.php'; ?>
