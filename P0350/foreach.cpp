template <size_t N>
void epilogue(ContiguousIterator first, ContiguousIterator last, UnaryFunction f);

template <>
inline void epilogue<0>(ContiguousIterator, ContiguousIterator, UnaryFunction) {}

template <size_t N>
inline void epilogue(ContiguousIterator first, ContiguousIterator last,
                     UnaryFunction f) {
  using V = datapar<ContiguousIterator::value_type, abi_for_size_t<N>>;
  V tmp = load<V>(std::addressof(*first), flags::unaligned);
  f(tmp);
  if (is_functor_argument_mutable<UnaryFunction, V>::value) {
    store(tmp, std::addressof(*first), flags::unaligned);
  }
  epilogue<V::size() / 2>(first, last, f);
}

void for_each(/*!\dataparEPT            {}*/, ContiguousIterator first,
              ContiguousIterator last, UnaryFunction f) {
  using V = datapar<ContiguousIterator::value_type, datapar_abi::native>;
  for (; distance(first, last) >= V::size(); first += V::size()) {
    V tmp = load<V>(std::addressof(*first), flags::unaligned);
    f(tmp);
    if (is_functor_argument_mutable<UnaryFunction, V>::value) {
      store(tmp, std::addressof(*first), flags::unaligned);
    }
  }
  epilogue<V::size() / 2>(first, last, f);
}
