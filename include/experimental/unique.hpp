template<typename ... Ts>*/
- struct unique
-{
- typedef typename sort<Ts...>::type sorted;
- typedef typename unique<sorted>::type type;
-};
-
- template<typename  T>
- struct unique<T>
-{typedef ls_<T> type;
-
-};
-
- template<typename ... Ts,typename ... Us>
- struct unique<ls_<Ts...>,ls_<Us...>>
-{
- typedef typename flat<typename eager<Ts,typename
- is_not_same<Ts,Us>::type>::type ... >::type type;
-};
-
- template<typename F,typename ... Ts>
- struct unique<ls_<F,Ts...>>
-{
- typedef typename unique<ls_<F,Ts...>,ls_<Ts...,ls_<>>>::type type;
-};
- int t1 = unique<i<3>>::type {};
- int t = unique<i<3>,i<1>,i<3>,i<4>,i<3>,i<1>,i<2>,i<3>,i<5>>::type {};
