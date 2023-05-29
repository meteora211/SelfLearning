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

main1 log:
```
[main1] before call coroutine func.
[coroutine] before co_await.
[Awaiter] In await_ready function
[Awaiter] In await_suspend function
[main1] after call coroutine func and fetch handle.
[main1] before call coroutine_handle<>().
[Awaiter] In await_resume function
[coroutine] after co_wait. counter: 0
[coroutine] before co_await.
[Awaiter] In await_ready function
[Awaiter] In await_suspend function
[main1] after call coroutine_handle<>().
[main1] before call coroutine_handle<>().
[Awaiter] In await_resume function
[coroutine] after co_wait. counter: 1
[coroutine] before co_await.
[Awaiter] In await_ready function
[Awaiter] In await_suspend function
[main1] after call coroutine_handle<>().
[main1] before call coroutine_handle<>().
[Awaiter] In await_resume function
[coroutine] after co_wait. counter: 2
[coroutine] before co_await.
[Awaiter] In await_ready function
[Awaiter] In await_suspend function
[main1] after call coroutine_handle<>().
```
