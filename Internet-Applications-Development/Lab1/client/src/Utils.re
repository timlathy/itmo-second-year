let chunkArray = (chunkSize: int, ary: array('a)): array(array('a)) => {
  let ret: array(array('a)) = [||];
  for (i in 0 to Js.Array.length(ary) - 1) {
    if (i mod chunkSize == 0) {
      let _ = Js.Array.push(Js.Array.slice(~start=i, ~end_=i + chunkSize, ary), ret);
    }
  }
  ret
}
