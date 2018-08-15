<table>
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

The point defined by (X, Y) is
<strong><?= $is_inside ? "inside" : "not inside" ?></strong> the polygon.
