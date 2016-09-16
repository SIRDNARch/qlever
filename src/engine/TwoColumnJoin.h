// Copyright 2016, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Author: Björn Buchhold (buchhold@informatik.uni-freiburg.de)
#pragma once

#include <list>

#include "./Operation.h"
#include "./QueryExecutionTree.h"

using std::list;



class TwoColumnJoin : public Operation {
public:

  TwoColumnJoin(QueryExecutionContext* qec, const QueryExecutionTree& t1,
                const QueryExecutionTree& t2,
                const std::vector<array<size_t, 2>>& joinCols);

  TwoColumnJoin(const TwoColumnJoin& other);

  TwoColumnJoin& operator=(const TwoColumnJoin& other);

  virtual ~TwoColumnJoin();

  virtual string asString() const;

  virtual size_t getResultWidth() const;

  virtual size_t resultSortedOn() const;

  ad_utility::HashMap<string, size_t> getVariableColumns() const;

  virtual void setTextLimit(size_t limit) {
    _left->setTextLimit(limit);
    _right->setTextLimit(limit);
  }

  virtual size_t getSizeEstimate() const {
    return (_left->getSizeEstimate() + _right->getSizeEstimate()) / 10;
  }

  virtual size_t getCostEstimate() const {
    if ((_left->getResultWidth() == 2 && _jc1Left == 0 && _jc2Left == 1) ||
       (_right->getResultWidth() == 2 && _jc1Right == 0 && _jc2Right == 1)) {
      return _left->getSizeEstimate() + _left->getCostEstimate() +
              _right->getSizeEstimate() + _right->getCostEstimate();
    }
    // PUNISH IF NO DIRECT JOIN IS AVAILABLE FOR FILTER
    return (_left->getSizeEstimate() + _left->getCostEstimate() +
           _right->getSizeEstimate() + _right->getCostEstimate()) * 1000;
  }

    virtual bool knownEmptyResult() const {
      return _left->knownEmptyResult() || _right->knownEmptyResult();
    }

private:
  QueryExecutionTree* _left;
  QueryExecutionTree* _right;

  size_t _jc1Left;
  size_t _jc2Left;
  size_t _jc1Right;
  size_t _jc2Right;

  virtual void computeResult(ResultTable* result) const;
};
