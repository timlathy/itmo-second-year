<section class="card">
  <table class="table">
    <thead>
      <tr>
        <td>Parameter</td>
        <td>Value</td>
      </tr>
    </thead>
    <tbody>
      <?php foreach ($params as $name => $val) { ?>
        <tr><td><?= $name ?></td><td><?= $val ?></td></tr>
      <?php } ?>
    </body>
  </table>
  <?php $result = $is_inside ? "inside" : "not inside" ?>
  <p>The point defined by (X, Y) is <strong><?= $result ?></strong> the polygon.</p>
  <p>Running time: <?= $running_time_micros ?>μs</p>
  <p>Server time: <?= $server_time ?></p>
</section>
