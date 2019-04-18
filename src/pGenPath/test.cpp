 if (m_first)
		 {
                   m_points.push_back(sval);
		   cout << "first" << endl;
		   m_first=false;
		 }
	       else
		 {
	           string x = tokStringParse(sval,"x",',','=');
                   string y = tokStringParse(sval,"y",',','=');
                   //string label = tokStringParse(working,"id",',','=');
	           //cout << "sval= " << sval.c_str() << endl;
                   double x_val = strtod(x.c_str(),NULL);
                   double y_val = strtod(y.c_str(),NULL);

	           m_dist = sqrt(pow(40-x_val,2)+pow(y_val,2));
                   vector<string>::iterator it;
	           for (it = m_points.begin();it!=m_points.end() && m_inserted!=true;it++)
		     {
		       string working = *it;
		       //cout << working.c_str() << endl;
		       string x_test = tokStringParse(sval,"x",',','=');
                       string y_test = tokStringParse(sval,"y",',','=');
                       //string label = tokStringParse(working,"id",',','=');
	  
                       double x_test_val = strtod(x.c_str(),NULL);
                       double y_test_val = strtod(y.c_str(),NULL);
                       
	               m_dist_test = sqrt(pow(40-x_test_val,2)+pow(y_test_val,2));
                       //cout << "before if" <<endl;
		       cout << m_dist << endl;
		       cout << m_dist_test << endl;
                       bool test = isless(m_dist,m_dist_test);
		       if (test)
		        {
                           cout << "test worked" << endl;
			   //m_points.insert(it, sval);
			   //m_inserted=true;
			   // }
			   // if(m_inserted!=true)
			   // {
			   // m_points.push_back(sval);
			   // m_inserted=false;
			    }
		     }
		 }
	     }
