/*******************************************************************************
 * The MIT License (MIT)
 * 
 * Copyright (c) 2018 Jean-David Gadina - www.xs-labs.com
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ******************************************************************************/

/*!
 * @header      Runner.hpp
 * @author      Jean-David Gadina - www.xs-labs.com
 * @copyright   (c) 2018, Jean-David Gadina - www.xs-labs.com
 */

#ifndef XS_TEST_RUNNER_HPP
#define XS_TEST_RUNNER_HPP

#include <iostream>
#include <memory>
#include <algorithm>
#include <XS/Test/Optional.hpp>
#include <XS/Test/Utility.hpp>
#include <XS/Test/Info.hpp>
#include <XS/Test/Suite.hpp>
#include <XS/Test/StopWatch.hpp>

namespace XS
{
    namespace Test
    {
        class Suite;
        
        class Runner
        {
            public:
                
                Runner( const std::vector< Suite > & suites ):
                    _suites( suites )
                {
                    Utility::Shuffle( this->_suites );
                }
                
                Runner( const Runner & o ):
                    Runner( o._suites )
                {}
                
                Runner( Runner && o ) noexcept:
                    _suites( std::move( o._suites ) )
                {}
                
                ~Runner( void )
                {}
                
                Runner & operator =( Runner o )
                {
                    swap( *( this ), o );
                    
                    return *( this );
                }
                
                std::vector< Suite > GetSuites( void ) const
                {
                    return this->_suites;
                }
                
                bool Run( Optional< std::reference_wrapper< std::ostream > > os )
                {
                    size_t    cases( 0 );
                    size_t    tests( 0 );
                    StopWatch time;
                    bool      success( true );
                    
                    if( this->_suites.size() == 0 )
                    {
                        return false;
                    }
                    
                    cases += this->_suites.size();
                    
                    for( const auto & suite: this->_suites )
                    {
                        tests += suite.GetInfos().size();
                    }
                    
                    if( os )
                    {
                        os.value().get() << "[==========] Running "
                                         << Utility::Numbered( "test", tests )
                                         << " from "
                                         << Utility::Numbered( "case", cases )
                                         << "."
                                         << std::endl;
                    }
                    
                    this->setup( os );
                    
                    time.Start();
                    
                    if( os )
                    {
                        os.value().get() << std::endl;
                    }
                    
                    for( auto & suite: this->_suites )
                    {
                        if( suite.Run( os ) == false )
                        {
                            success = false;
                        }
                    }
                    
                    time.Stop();
                    
                    this->tearDown( os );
                    
                    if( os )
                    {
                        os.value().get() << "[==========] "
                                         << Utility::Numbered( "test", tests )
                                         << " from "
                                         << Utility::Numbered( "case", cases )
                                         << " ran. ("
                                         << time.GetString()
                                         << " total)"
                                         << std::endl;
                    }
                    
                    {
                        std::vector< Info > passed;
                        std::vector< Info > failed;
                        
                        for( auto & suite: this->_suites )
                        {
                            for( auto & info: suite.GetInfos() )
                            {
                                if( info.GetStatus() == Info::Status::Failed )
                                {
                                    failed.push_back( info );
                                }
                                else if( info.GetStatus() == Info::Status::Success )
                                {
                                    passed.push_back( info );
                                }
                            }
                        }
                        
                        if( os )
                        {
                            os.value().get() << "[  PASSED  ] "
                                             << Utility::Numbered( "test", passed.size() )
                                             << "."
                                             << std::endl;
                            
                            if( failed.size() > 0 )
                            {
                                os.value().get() << "[  FAILED  ] "
                                                 << Utility::Numbered( "test", failed.size() )
                                                 << ", listed below:"
                                                 << std::endl;
                                
                                for( const auto & info: failed )
                                {
                                    os.value().get() << "[  FAILED  ] "
                                                     << info.GetName()
                                                     << std::endl;
                                }
                                
                                os.value().get() << std::endl
                                                 << Utility::Numbered( "FAILED TEST", failed.size(), "FAILED TESTS" )
                                                 << std::endl;
                            }
                        }
                    }
                    
                    return success;
                }
                
                friend void swap( Runner & o1, Runner & o2 ) noexcept
                {
                    using std::swap;
                    
                    swap( o1._suites, o2._suites );
                }
                
            private:
                
                void setup( Optional< std::reference_wrapper< std::ostream > > os )
                {
                    if( os )
                    {
                        os.value().get() << "[----------] Global test environment set-up." << std::endl;
                    }
                }
                
                void tearDown( Optional< std::reference_wrapper< std::ostream > > os )
                {
                    if( os )
                    {
                        os.value().get() << "[----------] Global test environment tear-down." << std::endl;
                    }
                }
                
                std::vector< Suite > _suites;
        };
    }
}

#endif /* XS_TEST_RUNNER_HPP */
