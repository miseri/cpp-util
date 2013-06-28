#pragma once

#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>

#include "ServiceThread.h"

/**
 * Utility functions to create service objects
 */
template< class T > 
boost::shared_ptr< ServiceThread<boost::shared_ptr<T> > > makeService()
{
  typedef boost::shared_ptr<T> ImplPtr_t;
  typedef ServiceThread< boost::shared_ptr<T> > ServiceImpl_t;
  typedef boost::shared_ptr<ServiceImpl_t> ServiceImplPtr_t;
  ImplPtr_t pServiceImpl = boost::make_shared<T>();
  ServiceImplPtr_t pService = boost::make_shared<ServiceImpl_t>();
  pService->set(pServiceImpl);
  return pService;
}

template< class T, class A1 > 
boost::shared_ptr< ServiceThread<boost::shared_ptr<T> > > makeService(A1 const & a1)
{
  typedef boost::shared_ptr<T> ImplPtr_t;
  typedef ServiceThread< boost::shared_ptr<T> > ServiceImpl_t;
  typedef boost::shared_ptr<ServiceImpl_t> ServiceImplPtr_t;
  ImplPtr_t pServiceImpl = boost::make_shared<T>(a1);
  ServiceImplPtr_t pService = boost::make_shared<ServiceImpl_t>();
  pService->set(pServiceImpl);
  return pService;
}

template< class T, class A1, class A2 > 
boost::shared_ptr< ServiceThread<boost::shared_ptr<T> > > makeService(A1 const & a1, A2 const & a2)
{
  typedef boost::shared_ptr<T> ImplPtr_t;
  typedef ServiceThread< boost::shared_ptr<T> > ServiceImpl_t;
  typedef boost::shared_ptr<ServiceImpl_t> ServiceImplPtr_t;
  ImplPtr_t pServiceImpl = boost::make_shared<T>(a1, a2);
  ServiceImplPtr_t pService = boost::make_shared<ServiceImpl_t>();
  pService->set(pServiceImpl);
  return pService;
}

template< class T, class A1, class A2, class A3 > 
boost::shared_ptr< ServiceThread<boost::shared_ptr<T> > > makeService(A1 const & a1, A2 const & a2, A3 const & a3)
{
  typedef boost::shared_ptr<T> ImplPtr_t;
  typedef ServiceThread< boost::shared_ptr<T> > ServiceImpl_t;
  typedef boost::shared_ptr<ServiceImpl_t> ServiceImplPtr_t;
  ImplPtr_t pServiceImpl = boost::make_shared<T>(a1, a2, a3);
  ServiceImplPtr_t pService = boost::make_shared<ServiceImpl_t>();
  pService->set(pServiceImpl);
  return pService;
}

template< class T, class A1, class A2, class A3, class A4 > 
boost::shared_ptr< ServiceThread<boost::shared_ptr<T> > > makeService(A1 const & a1, A2 const & a2, A3 const & a3, A4 const & a4)
{
  typedef boost::shared_ptr<T> ImplPtr_t;
  typedef ServiceThread< boost::shared_ptr<T> > ServiceImpl_t;
  typedef boost::shared_ptr<ServiceImpl_t> ServiceImplPtr_t;
  ImplPtr_t pServiceImpl = boost::make_shared<T>(a1, a2, a3, a4);
  ServiceImplPtr_t pService = boost::make_shared<ServiceImpl_t>();
  pService->set(pServiceImpl);
  return pService;
}

template< class T, class A1, class A2, class A3, class A4, class A5 > 
boost::shared_ptr< ServiceThread<boost::shared_ptr<T> > > makeService(A1 const & a1, A2 const & a2, A3 const & a3, A4 const & a4, A5 const & a5)
{
  typedef boost::shared_ptr<T> ImplPtr_t;
  typedef ServiceThread< boost::shared_ptr<T> > ServiceImpl_t;
  typedef boost::shared_ptr<ServiceImpl_t> ServiceImplPtr_t;
  ImplPtr_t pServiceImpl = boost::make_shared<T>(a1, a2, a3, a4, a5);
  ServiceImplPtr_t pService = boost::make_shared<ServiceImpl_t>();
  pService->set(pServiceImpl);
  return pService;
}

template< class T, class A1, class A2, class A3, class A4, class A5, class A6 > 
boost::shared_ptr< ServiceThread<boost::shared_ptr<T> > > makeService(A1 const & a1, A2 const & a2, A3 const & a3, A4 const & a4, A5 const & a5, A6 const & a6)
{
  typedef boost::shared_ptr<T> ImplPtr_t;
  typedef ServiceThread< boost::shared_ptr<T> > ServiceImpl_t;
  typedef boost::shared_ptr<ServiceImpl_t> ServiceImplPtr_t;
  ImplPtr_t pServiceImpl = boost::make_shared<T>(a1, a2, a3, a4, a5, a6);
  ServiceImplPtr_t pService = boost::make_shared<ServiceImpl_t>();
  pService->set(pServiceImpl);
  return pService;
}

template< class T, class A1, class A2, class A3, class A4, class A5, class A6, class A7 >
boost::shared_ptr< ServiceThread<boost::shared_ptr<T> > > makeService(A1 const & a1, A2 const & a2, A3 const & a3, A4 const & a4, A5 const & a5, A6 const & a6, A7 const & a7)
{
  typedef boost::shared_ptr<T> ImplPtr_t;
  typedef ServiceThread< boost::shared_ptr<T> > ServiceImpl_t;
  typedef boost::shared_ptr<ServiceImpl_t> ServiceImplPtr_t;
  ImplPtr_t pServiceImpl = boost::make_shared<T>(a1, a2, a3, a4, a5, a6, a7);
  ServiceImplPtr_t pService = boost::make_shared<ServiceImpl_t>();
  pService->set(pServiceImpl);
  return pService;
}

template< class T, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8 >
boost::shared_ptr< ServiceThread<boost::shared_ptr<T> > > makeService(A1 const & a1, A2 const & a2, A3 const & a3, A4 const & a4, A5 const & a5, A6 const & a6, A7 const & a7, A8 const & a8)
{
  typedef boost::shared_ptr<T> ImplPtr_t;
  typedef ServiceThread< boost::shared_ptr<T> > ServiceImpl_t;
  typedef boost::shared_ptr<ServiceImpl_t> ServiceImplPtr_t;
  ImplPtr_t pServiceImpl = boost::make_shared<T>(a1, a2, a3, a4, a5, a6, a7, a8);
  ServiceImplPtr_t pService = boost::make_shared<ServiceImpl_t>();
  pService->set(pServiceImpl);
  return pService;
}
