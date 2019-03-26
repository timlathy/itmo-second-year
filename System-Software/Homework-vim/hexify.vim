function Hexify()
  let line_idx = 1

  while line_idx != 0
    let line = getline(line_idx)

    while 1
      let numbers = matchlist(line, '\<\d\+\>')
      if empty(numbers)
        break
      endif
      
      let number_dec = str2nr(numbers[0], 10)
      let line = substitute(line, numbers[0], printf("0x%x", number_dec), "g")

      call setline(line_idx, line)
    endwhile

    let line_idx = nextnonblank(line_idx + 1)
  endwhile
endfunction
