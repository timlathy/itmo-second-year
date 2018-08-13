<?php include __DIR__ . '/../_head.html.php'; ?>

<form id="line-input-form">
  <input type="text" name="graph-name" placeholder="Graph name" />
  <div id="line-input-container"></div>
  <label for="line-input-variables">Preview variables</label>
  <input type="text" id="line-input-variables" name="variables" placeholder="R = 2.3, S = ..." />
  <button id="line-input-preview">Preview</button>
  <button id="line-input-save" type="submit">Save</button>
  <div id="line-input-preview-container"></div>
</form>

<?php include __DIR__ . '/../_footer.html.php'; ?>
