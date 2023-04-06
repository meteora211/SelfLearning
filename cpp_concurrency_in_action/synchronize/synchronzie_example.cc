#include <iostream>
#include <future>
#include <thread>
#include <algorithm>
#include <vector>
#include <iterator>
#include <type_traits>
/* #include <experimental/future> */
#include <barrier>
#include <latch>
#include <string>

template<typename T>
void print_stl(const T& v) {
  for (auto&& elem : v) {
    std::cout << elem << " ";
  }
  std::cout << std::endl;
}

template<typename ForwardIt>
void print_stl(const ForwardIt& first, const ForwardIt& last) {
  for (auto it = first; it != last; ++it) {
    std::cout << *it << " ";
  }
  std::cout << std::endl;
}

/* template<typename F> */
/* auto experimental_async(F&& f) { */
/*   std::experimental::promise<std::result_of<F()>> ex_p; */
/*   auto ex_f = ex_p.get_future(); */

/*   t = std::thread([p = std::move(ex_p), rf = std::move(f)]() mutable { */
/*         try { */
/*           p.set_value_at_thread_exit(rf()); */
/*         } catch { */
/*           p.set_exception_at_thread_exit(); */
/*         } */
/*       }); */
/*   t.detach(); */

/*   return ex_f; */
/* } */

/* int experimental_func(std::experimental::future<int>) { */
/*   std::cout << "experimental func" << std::endl; */
/*   return 42; */
/* } */

template<typename ForwardIt>
void quick_sort(ForwardIt first, ForwardIt last, bool use_async = false) {
  if (first == last) return;

  auto pivot = *std::next(first, std::distance(first, last) / 2);

  auto middle1 = std::partition(first, last, [&](const auto& elem) { return elem < pivot; });
  auto middle2 = std::partition(middle1, last, [&](const auto& elem) { return elem == pivot; });
  /* std::cout << pivot << std::endl; */
  /* print_stl(first, middle1); */
  /* print_stl(middle2, last); */
  std::cout << std::this_thread::get_id() << std::endl;

  quick_sort(first, middle1);
  std::future<void> f;
  if (use_async) {
    f = std::async(std::launch::async, quick_sort<ForwardIt>, middle2, last, use_async);
  } else {
    f = std::async(std::launch::deferred, quick_sort<ForwardIt>, middle2, last, use_async);
  }
  // XXX: do not forget get()/wait()
  f.get();
}

int main() {

  {
    std::vector<int> v = {1, 30, -4, 3, 5, -4, 1, 6, -8, 2, -5, 64, 1, 92};
    std::cout << "async thread" << std::endl;
    quick_sort(v.begin(), v.end(), true);
    print_stl(v);
    std::cout << "deferred thread" << std::endl;
    quick_sort(v.begin(), v.end(), false);
    print_stl(v);
  }
  /* { */
  /*   std::experimental::future<int> eft; */
  /*   auto f1 = eft(); */
  /*   auto ft2 = ft1.then(experimental_func); */
  /*   std::cout << ft2.get() << std::endl; */
  /* } */
  {
    // barrier example
    std::cout << "Barrier Start \n";
    const auto workers = {"Anil", "Busara", "Carl"};

    auto compiletion = [](){
      static auto phase ="... done\n" "Cleanning up...\n";
      std::cout << phase;
      phase = "...done\n";
    };

    std::barrier sync_point(std::size(workers), compiletion);
    auto work = [&sync_point](const std::string& name){
      std::string product = "    " + name + "working... \n";
      std::cout << product;

      // sync_point.arrive_and_wait();
      auto&& token = sync_point.arrive();
      sync_point.wait(std::move(token));

      std::string clean = "    " + name + "cleaning... \n";
      std::cout << clean;

      sync_point.arrive_and_wait();
    };

    std::vector<std::jthread> threads;
    for (const auto& worker : workers) {
      threads.emplace_back(work, worker);
    }
  }
  {
    // latch example
    struct job {
      const std::string name;
      std::string product{"not working"};
      std::thread action{};
    } jobs[] = {{"annika"}, {"buru"}, {"chuck"}};

    std::latch work_done{std::size(jobs)};
    std::latch clean_up{1};

    auto work = [&](job& j){
      j.product = j.name + " worked\n";
      work_done.count_down();
      clean_up.wait();
      j.product = j.name + " cleaned\n";
    };

    std::cout << "Latch Start \n";
    for (auto& j : jobs) {
      j.action = std::thread(work, std::ref(j));
    }
    work_done.wait();
    for (auto& j : jobs) {
      std::cout << "    " << j.product;
    }
    std::cout << "...done\n";
    clean_up.count_down();
    std::cout << "...Cleaning up\n";
    for (auto& j : jobs) {
      // XXX: not using jthread because we need wait thread done here to print cleaned
      j.action.join();
      std::cout << "    " << j.product;
    }
    std::cout << "...done\n";
  }

}
