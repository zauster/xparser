Tests for XParser
Author: Sander van der Hoog
Date: 18 Jan 2010

These tests are meant for checking Libmboard message operations:

Models:
- sorting
- filtering
- randomizing
- filtering and randomizing

1. Message sorting

      <input>
       <messageName>info</messageName>
       <sort>
        <key>id</key>
        <order>descend</order>
       </sort>
      </input>

2. Message filtering

      <input>
       <messageName>info</messageName>
       <filter>
       <lhs>
       <value>m.id</value>
       </lhs>
       <op>GEQ</op>
       <rhs>
       <value>3</value>
       </rhs>
       </filter>
      </input>


3. Message randomizing

      <input>
       <messageName>info</messageName>
       <random>true</random>
      </input>

4. Message filtering and randomizing

      <input>
       <messageName>info</messageName>
       <random>true</random>
       <filter>
       <lhs>
       <value>m.id</value>
       </lhs>
       <op>GEQ</op>
       <rhs>
       <value>3</value>
       </rhs>
       </filter>
      </input>
