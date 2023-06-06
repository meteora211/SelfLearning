`co_wait <expr>` is able to expand to:
```
{
  auto&& tmp = <expr>;
  if (!tmp.await_ready()) {
    tmp.await_suspend(<coroutine-handle>);
  }
  return tmp.await_resume();
}
```

more readings from lewissbaker:
- https://lewissbaker.github.io/2017/09/25/coroutine-theory
- https://lewissbaker.github.io/2017/11/17/understanding-operator-co-await
- https://lewissbaker.github.io/2018/09/05/understanding-the-promise-type
- https://lewissbaker.github.io/2020/05/11/understanding_symmetric_transfer
- https://lewissbaker.github.io/2022/08/27/understanding-the-compiler-transform
