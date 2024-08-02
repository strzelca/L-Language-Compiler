function fib(x) {
  if (x < 3) {
    return 1;
  } else {
    return fib(x-1)+fib(x-2);
  }
}

function loop(x) {
  while (x < 10) {
    printd(x);
  }
}

loop(5);
printd(fib(10));
