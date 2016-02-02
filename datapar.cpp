namespace std {
  namespace experimental {
    template <class T, class Target = datapar_target::compatible> class datapar {
    public:
      typedef implementation_defined native_handle_type;
      typedef T value_type;
      typedef implementation_defined register_value_type;
      typedef implementation_defined reference;
      typedef implementation_defined const_reference;
      typedef mask<T, Target> mask_type;
      typedef size_t size_type;
      typedef Target target_type;

      template <class U = T> static constexpr size_t memory_alignment = implementation_defined;

      static constexpr size_type size();

      datapar() = default;

      datapar(const datapar &) = default;
      datapar(datapar &&) = default;
      datapar &operator=(const datapar &) = default;
      datapar &operator=(datapar &&) = default;

      // implicit broadcast constructor
      datapar(value_type);

      // implicit type conversion constructor
      template <class U> datapar(datapar<U, Target>);

      // loads:
      static datapar load(const value_type *);
      template <class Flags> static datapar load(const value_type *, Flags);
      template <class U, class Flags = unaligned_tag> static datapar load(const U *, Flags = Flags());

      // stores:
      void store(value_type *);
      template <class Flags> void store(value_type *, Flags);
      template <class U, class Flags = unaligned_tag> void store(U *, Flags = Flags());

      // masked stores:
      void store(value_type *, mask_type);
      template <class Flags> void store(value_type *, mask_type, Flags);
      template <class U, class Flags = unaligned_tag> void store(U *, mask_type, Flags = Flags());

      // scalar access:
      reference operator[](size_type);
      const_reference operator[](size_type) const;

      // increment and decrement:
      datapar &operator++();
      datapar operator++(int);
      datapar &operator--();
      datapar operator--(int);

      // unary operators (for integral T)
      mask_type operator!() const;
      datapar operator~() const;

      // unary operators (for any T)
      datapar operator+() const;
      datapar operator-() const;

      // access to the internals for implementation-specific extensions
      native_handle_type &native_handle();
      const native_handle_type &native_handle() const;
    };

    template <class T, class Target>
    template <class U>
    constexpr size_t datapar<T, Target>::memory_alignment<U>;
  }
}