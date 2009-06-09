    /**
     *  When queries accept predicates, they subclass this class.
     */
    template                       <typename ObjectType>
    class                          HavePredicate
      : public DynamicPredicateVisitor<ObjectType>
    {
     private:
      boost::shared_ptr<Predicate> predicate_;

     public:
      /**
       *  HavePredicate default constructor.
       */
                                   HavePredicate() {}

      /**
       *  HavePredicate copy constructor.
       */
                                   HavePredicate(const HavePredicate& hp)
        : DynamicPredicateVisitor<ObjectType>(hp)
      {
        this->predicate_ = hp.predicate_;
      }

      /**
       *  HavePredicate destructor.
       */
      virtual                      ~HavePredicate() {}

      /**
       *  HavePredicate operator= overload.
       */
      HavePredicate&               operator=(const HavePredicate& hp)
      {
	this->DynamicPredicateVisitor<ObjectType>::operator=(hp);
	this->predicate_ = hp.predicate_;
	return (*this);
      }

      /**
       *  Set the predicate.
       */
      template                     <typename PredicateType>
      void                         SetPredicate(const PredicateType& predicate)
      {
	this->predicate_ = new PredicateType(predicate);
	return ;
      }
    };

