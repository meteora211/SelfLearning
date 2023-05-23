I don't think this example works as expected. but just write it done for exercise.

Some problem:
- yield between threads will cause cpu utilization up to 100% when there's no task to execute.
- local_queue_ examples doesn't work as expected. Becuase submit function is always called in mainthread, which local_queue_ could always be nullptr.
- existing(dtor) only set done to true. the while loop in worker thread of the book only check the done flag, and doesn't consider to executes the remained tasks.
