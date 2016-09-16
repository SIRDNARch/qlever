// Copyright 2015, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Author: Björn Buchhold (buchhold@informatik.uni-freiburg.de)

#include <sstream>
#include "./QueryExecutionTree.h"
#include "./Join.h"
#include "Sort.h"

using std::string;

// _____________________________________________________________________________
Join::Join(QueryExecutionContext *qec,
           const QueryExecutionTree& t1,
           const QueryExecutionTree& t2,
           size_t t1JoinCol,
           size_t t2JoinCol,
           bool keepJoinColumn) : Operation(qec) {
  // Make sure subtrees are ordered so that identical queries can be identified.
  if (t1.asString() < t2.asString()) {
    _left = new QueryExecutionTree(t1);
    _leftJoinCol = t1JoinCol;
    _right = new QueryExecutionTree(t2);
    _rightJoinCol = t2JoinCol;
  } else {
    _left = new QueryExecutionTree(t2);
    _leftJoinCol = t2JoinCol;
    _right = new QueryExecutionTree(t1);
    _rightJoinCol = t1JoinCol;
  }
  _keepJoinColumn = keepJoinColumn;
}

// _____________________________________________________________________________
Join::~Join() {
  delete _left;
  delete _right;
}


// _____________________________________________________________________________
Join::Join(const Join& other) :
    Operation(other._executionContext),
    _left(new QueryExecutionTree(*other._left)),
    _right(new QueryExecutionTree(*other._right)),
    _leftJoinCol(other._leftJoinCol),
    _rightJoinCol(other._rightJoinCol),
    _keepJoinColumn(other._keepJoinColumn) {
}

// _____________________________________________________________________________
Join& Join::operator=(const Join& other) {
  _executionContext = other._executionContext;
  delete _left;
  _left = new QueryExecutionTree(*other._left);
  delete _right;
  _right = new QueryExecutionTree(*other._right);
  _leftJoinCol = other._leftJoinCol;
  _rightJoinCol = other._rightJoinCol;
  _keepJoinColumn = other._keepJoinColumn;
  return *this;
}

// _____________________________________________________________________________
string Join::asString() const {
  std::ostringstream os;
  os << "JOIN(\n" << _left->asString() << " [" << _leftJoinCol <<
  "]\n|X|\n" << _right->asString() << " [" << _rightJoinCol << "]\n)";
  return os.str();
}

// _____________________________________________________________________________
void Join::computeResult(ResultTable *result) const {
  LOG(DEBUG) << "Join result computation..." << endl;
  size_t leftWidth = _left->getResultWidth();
  size_t rightWidth = _right->getResultWidth();

  // Checking this before calling getResult on the subtrees can
  // avoid the computation of an non-empty subtree.
  if (_left->knownEmptyResult() || _right->knownEmptyResult()) {
    size_t resWidth = leftWidth + rightWidth - 1;
    if (resWidth == 1) {
      result->_fixedSizeData = new vector<array<Id, 1>>();
    } else if (resWidth == 2) {
      result->_fixedSizeData = new vector<array<Id, 2>>();
    } else if (resWidth == 3) {
      result->_fixedSizeData = new vector<array<Id, 3>>();
    } else if (resWidth == 4) {
      result->_fixedSizeData = new vector<array<Id, 4>>();
    } else if (resWidth == 5) {
      result->_fixedSizeData = new vector<array<Id, 5>>();
    }
    result->_status = ResultTable::FINISHED;
    return;
  }

  const ResultTable& leftRes = _left->getRootOperation()->getResult();
  const ResultTable& rightRes = _right->getRootOperation()->getResult();

  AD_CHECK(result);
  AD_CHECK(!result->_fixedSizeData);

  result->_nofColumns = leftWidth + rightWidth - 1;
  result->_sortedBy = _leftJoinCol;

  if (leftWidth == 1) {
    if (rightWidth == 1) {
      result->_fixedSizeData = new vector<array<Id, 1>>();
      _executionContext->getEngine().join(
          *static_cast<const vector<array<Id, 1>> *>(leftRes._fixedSizeData),
          _leftJoinCol,
          *static_cast<const vector<array<Id, 1>> *>(rightRes._fixedSizeData),
          _rightJoinCol,
          static_cast<vector<array<Id, 1>> *>(result->_fixedSizeData));
    } else if (rightWidth == 2) {
      result->_fixedSizeData = new vector<array<Id, 2>>();
      _executionContext->getEngine().join(
          *static_cast<const vector<array<Id, 1>> *>(leftRes._fixedSizeData),
          _leftJoinCol,
          *static_cast<const vector<array<Id, 2>> *>(rightRes._fixedSizeData),
          _rightJoinCol,
          static_cast<vector<array<Id, 2>> *>(result->_fixedSizeData));
    } else if (rightWidth == 3) {
      result->_fixedSizeData = new vector<array<Id, 3>>();
      _executionContext->getEngine().join(
          *static_cast<const vector<array<Id, 1>> *>(leftRes._fixedSizeData),
          _leftJoinCol,
          *static_cast<const vector<array<Id, 3>> *>(rightRes._fixedSizeData),
          _rightJoinCol,
          static_cast<vector<array<Id, 3>> *>(result->_fixedSizeData));
    } else if (rightWidth == 4) {
      result->_fixedSizeData = new vector<array<Id, 4>>();
      _executionContext->getEngine().join(
          *static_cast<const vector<array<Id, 1>> *>(leftRes._fixedSizeData),
          _leftJoinCol,
          *static_cast<const vector<array<Id, 4>> *>(rightRes._fixedSizeData),
          _rightJoinCol,
          static_cast<vector<array<Id, 4>> *>(result->_fixedSizeData));
    } else if (rightWidth == 5) {
      result->_fixedSizeData = new vector<array<Id, 5>>();
      _executionContext->getEngine().join(
          *static_cast<const vector<array<Id, 1>> *>(leftRes._fixedSizeData),
          _leftJoinCol,
          *static_cast<const vector<array<Id, 5>> *>(rightRes._fixedSizeData),
          _rightJoinCol,
          static_cast<vector<array<Id, 5>> *>(result->_fixedSizeData));
    } else {
      _executionContext->getEngine().join(
          *static_cast<const vector<array<Id, 1>> *>(leftRes._fixedSizeData),
          _leftJoinCol,
          rightRes._varSizeData,
          _rightJoinCol,
          &result->_varSizeData);
    }
  } else if (leftWidth == 2) {
    if (rightWidth == 1) {
      result->_fixedSizeData = new vector<array<Id, 2>>();
      _executionContext->getEngine().join(
          *static_cast<const vector<array<Id, 2>> *>(leftRes._fixedSizeData),
          _leftJoinCol,
          *static_cast<const vector<array<Id, 1>> *>(rightRes._fixedSizeData),
          _rightJoinCol,
          static_cast<vector<array<Id, 2>> *>(result->_fixedSizeData));;
    } else if (rightWidth == 2) {
      result->_fixedSizeData = new vector<array<Id, 3>>();
      _executionContext->getEngine().join(
          *static_cast<const vector<array<Id, 2>> *>(leftRes._fixedSizeData),
          _leftJoinCol,
          *static_cast<const vector<array<Id, 2>> *>(rightRes._fixedSizeData),
          _rightJoinCol,
          static_cast<vector<array<Id, 3>> *>(result->_fixedSizeData));
    } else if (rightWidth == 3) {
      result->_fixedSizeData = new vector<array<Id, 4>>();
      _executionContext->getEngine().join(
          *static_cast<const vector<array<Id, 2>> *>(leftRes._fixedSizeData),
          _leftJoinCol,
          *static_cast<const vector<array<Id, 3>> *>(rightRes._fixedSizeData),
          _rightJoinCol,
          static_cast<vector<array<Id, 4>> *>(result->_fixedSizeData));
    } else if (rightWidth == 4) {
      result->_fixedSizeData = new vector<array<Id, 5>>();
      _executionContext->getEngine().join(
          *static_cast<const vector<array<Id, 2>> *>(leftRes._fixedSizeData),
          _leftJoinCol,
          *static_cast<const vector<array<Id, 4>> *>(rightRes._fixedSizeData),
          _rightJoinCol,
          static_cast<vector<array<Id, 5>> *>(result->_fixedSizeData));
    } else if (rightWidth == 5) {
      _executionContext->getEngine().join(
          *static_cast<const vector<array<Id, 2>> *>(leftRes._fixedSizeData),
          _leftJoinCol,
          *static_cast<const vector<array<Id, 5>> *>(rightRes._fixedSizeData),
          _rightJoinCol,
          &result->_varSizeData);
    } else {
      _executionContext->getEngine().join(
          *static_cast<const vector<array<Id, 2>> *>(leftRes._fixedSizeData),
          _leftJoinCol,
          rightRes._varSizeData,
          _rightJoinCol,
          &result->_varSizeData);
    }
  } else if (leftWidth == 3) {
    if (rightWidth == 1) {
      result->_fixedSizeData = new vector<array<Id, 3>>();
      _executionContext->getEngine().join(
          *static_cast<const vector<array<Id, 3>> *>(leftRes._fixedSizeData),
          _leftJoinCol,
          *static_cast<const vector<array<Id, 1>> *>(rightRes._fixedSizeData),
          _rightJoinCol,
          static_cast<vector<array<Id, 3>> *>(result->_fixedSizeData));
    } else if (rightWidth == 2) {
      result->_fixedSizeData = new vector<array<Id, 4>>();
      _executionContext->getEngine().join(
          *static_cast<const vector<array<Id, 3>> *>(leftRes._fixedSizeData),
          _leftJoinCol,
          *static_cast<const vector<array<Id, 2>> *>(rightRes._fixedSizeData),
          _rightJoinCol,
          static_cast<vector<array<Id, 4>> *>(result->_fixedSizeData));
    } else if (rightWidth == 3) {
      result->_fixedSizeData = new vector<array<Id, 5>>();
      _executionContext->getEngine().join(
          *static_cast<const vector<array<Id, 3>> *>(leftRes._fixedSizeData),
          _leftJoinCol,
          *static_cast<const vector<array<Id, 3>> *>(rightRes._fixedSizeData),
          _rightJoinCol,
          static_cast<vector<array<Id, 5>> *>(result->_fixedSizeData));
    } else if (rightWidth == 4) {
      _executionContext->getEngine().join(
          *static_cast<const vector<array<Id, 3>> *>(leftRes._fixedSizeData),
          _leftJoinCol,
          *static_cast<const vector<array<Id, 4>> *>(rightRes._fixedSizeData),
          _rightJoinCol,
          &result->_varSizeData);
    } else if (rightWidth == 5) {
      _executionContext->getEngine().join(
          *static_cast<const vector<array<Id, 3>> *>(leftRes._fixedSizeData),
          _leftJoinCol,
          *static_cast<const vector<array<Id, 5>> *>(rightRes._fixedSizeData),
          _rightJoinCol,
          &result->_varSizeData);
    } else {
      _executionContext->getEngine().join(
          *static_cast<const vector<array<Id, 3>> *>(leftRes._fixedSizeData),
          _leftJoinCol,
          rightRes._varSizeData,
          _rightJoinCol,
          &result->_varSizeData);
    }
  } else if (leftWidth == 4) {
    if (rightWidth == 1) {
      result->_fixedSizeData = new vector<array<Id, 4>>();
      _executionContext->getEngine().join(
          *static_cast<const vector<array<Id, 4>> *>(leftRes._fixedSizeData),
          _leftJoinCol,
          *static_cast<const vector<array<Id, 1>> *>(rightRes._fixedSizeData),
          _rightJoinCol,
          static_cast<vector<array<Id, 4>> *>(result->_fixedSizeData));
    } else if (rightWidth == 2) {
      result->_fixedSizeData = new vector<array<Id, 5>>();
      _executionContext->getEngine().join(
          *static_cast<const vector<array<Id, 4>> *>(leftRes._fixedSizeData),
          _leftJoinCol,
          *static_cast<const vector<array<Id, 2>> *>(rightRes._fixedSizeData),
          _rightJoinCol,
          static_cast<vector<array<Id, 5>> *>(result->_fixedSizeData));
    } else if (rightWidth == 3) {
      _executionContext->getEngine().join(
          *static_cast<const vector<array<Id, 4>> *>(leftRes._fixedSizeData),
          _leftJoinCol,
          *static_cast<const vector<array<Id, 3>> *>(rightRes._fixedSizeData),
          _rightJoinCol,
          &result->_varSizeData);
    } else if (rightWidth == 4) {
      _executionContext->getEngine().join(
          *static_cast<const vector<array<Id, 4>> *>(leftRes._fixedSizeData),
          _leftJoinCol,
          *static_cast<const vector<array<Id, 4>> *>(rightRes._fixedSizeData),
          _rightJoinCol,
          &result->_varSizeData);
    } else if (rightWidth == 5) {
      _executionContext->getEngine().join(
          *static_cast<const vector<array<Id, 4>> *>(leftRes._fixedSizeData),
          _leftJoinCol,
          *static_cast<const vector<array<Id, 5>> *>(rightRes._fixedSizeData),
          _rightJoinCol,
          &result->_varSizeData);
    } else {
      _executionContext->getEngine().join(
          *static_cast<const vector<array<Id, 4>> *>(leftRes._fixedSizeData),
          _leftJoinCol,
          rightRes._varSizeData,
          _rightJoinCol,
          &result->_varSizeData);
    }
  } else if (leftWidth == 5) {
    if (rightWidth == 1) {
      result->_fixedSizeData = new vector<array<Id, 5>>();
      _executionContext->getEngine().join(
          *static_cast<const vector<array<Id, 5>> *>(leftRes._fixedSizeData),
          _leftJoinCol,
          *static_cast<const vector<array<Id, 1>> *>(rightRes._fixedSizeData),
          _rightJoinCol,
          static_cast<vector<array<Id, 5>> *>(result->_fixedSizeData));
    } else if (rightWidth == 2) {
      _executionContext->getEngine().join(
          *static_cast<const vector<array<Id, 5>> *>(leftRes._fixedSizeData),
          _leftJoinCol,
          *static_cast<const vector<array<Id, 2>> *>(rightRes._fixedSizeData),
          _rightJoinCol,
          &result->_varSizeData);
    } else if (rightWidth == 3) {
      _executionContext->getEngine().join(
          *static_cast<const vector<array<Id, 5>> *>(leftRes._fixedSizeData),
          _leftJoinCol,
          *static_cast<const vector<array<Id, 3>> *>(rightRes._fixedSizeData),
          _rightJoinCol,
          &result->_varSizeData);
    } else if (rightWidth == 4) {
      _executionContext->getEngine().join(
          *static_cast<const vector<array<Id, 5>> *>(leftRes._fixedSizeData),
          _leftJoinCol,
          *static_cast<const vector<array<Id, 4>> *>(rightRes._fixedSizeData),
          _rightJoinCol,
          &result->_varSizeData);
    } else if (rightWidth == 5) {
      _executionContext->getEngine().join(
          *static_cast<const vector<array<Id, 5>> *>(leftRes._fixedSizeData),
          _leftJoinCol,
          *static_cast<const vector<array<Id, 5>> *>(rightRes._fixedSizeData),
          _rightJoinCol,
          &result->_varSizeData);
    } else {
      _executionContext->getEngine().join(
          *static_cast<const vector<array<Id, 5>> *>(leftRes._fixedSizeData),
          _leftJoinCol,
          rightRes._varSizeData,
          _rightJoinCol,
          &result->_varSizeData);
    }
  } else {
    _executionContext->getEngine().join(
        leftRes._varSizeData,
        _leftJoinCol,
        rightRes._varSizeData,
        _rightJoinCol,
        &result->_varSizeData);
  }
  result->_status = ResultTable::FINISHED;
  LOG(DEBUG) << "Join result computation done." << endl;
}

// _____________________________________________________________________________
ad_utility::HashMap<string, size_t> Join::getVariableColumns() const {
  ad_utility::HashMap<string, size_t> retVal(_left->getVariableColumnMap());
  size_t leftSize = _left->getResultWidth();
  for (auto it = _right->getVariableColumnMap().begin();
       it != _right->getVariableColumnMap().end(); ++it) {
    if (it->second < _rightJoinCol) {
      retVal[it->first] = leftSize + it->second;
    }
    if (it->second > _rightJoinCol) {
      retVal[it->first] = leftSize + it->second - 1;
    }
  }
  return retVal;
}

// _____________________________________________________________________________
size_t Join::getResultWidth() const {
  size_t res = _left->getResultWidth() + _right->getResultWidth() -
               (_keepJoinColumn ? 1 : 2);
  AD_CHECK(res > 0);
  return res;
}

// _____________________________________________________________________________
size_t Join::resultSortedOn() const {
  return _leftJoinCol;
}

// _____________________________________________________________________________
bool Join::isSelfJoin() const {
  return _left->asString() == _right->asString();
}

// _____________________________________________________________________________
ad_utility::HashSet<string> Join::getContextVars() const {
  auto cvars = _left->getContextVars();
  cvars.insert(_right->getContextVars().begin(),
               _right->getContextVars().end());
  return cvars;
}


