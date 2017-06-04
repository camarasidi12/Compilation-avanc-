	#include <Function.h>
	
	Function::Function(){
	  _head = NULL;
	  _end = NULL;
	  BB_computed = false;
	  BB_pred_succ = false;
	  dom_computed = false;
	}
	
	Function::~Function(){}
	
	void Function::set_head(Line *head){
	  _head = head;
	}
	
	void Function::set_end(Line *end){
	  _end = end;
	}
	
	Line* Function::get_head(){
	  return _head;
	}
	
	Basic_block* Function::get_firstBB(){
	   return _myBB.front();
	}
	
	Line* Function::get_end(){
	  return _end;
	}
	void Function::display(){
	  cout<<"Begin Function"<<endl;
	  Line* element = _head;
	
	  if(element == _end)	
	    cout << _head->get_content() <<endl;
	
	  while(element != _end){
	    cout << element->get_content() <<endl;
			
	    if(element->get_next()==_end){
	      cout << element->get_next()->get_content() <<endl;
	      break;
	    }
	    else element = element->get_next();
	     
	    }
	  cout<<"End Function\n\n"<<endl;
		
	}
	
	int Function::size(){
	  Line* element = _head;
	  int lenght=0;
	  while(element != _end)
	    {
	      lenght++;		
	      if (element->get_next()==_end)
		break;
	      else
		element = element->get_next();
	    }
	  return lenght;
	}	
	
	
	void Function::restitution(string const filename){
		
	  Line* element = _head;
	  ofstream monflux(filename.c_str(), ios::app);
	
	  if(monflux){
	    monflux<<"Begin"<<endl;
	    if(element == _end)	
	      monflux << _head->get_content() <<endl;
	    while(element != _end)
	      {
		if(element->isInst() || 
		   element->isDirective()) 
		  monflux<<"\t";
		
		monflux << element->get_content() ;
		
		if(element->get_content().compare("nop")) 
		  monflux<<endl;
			
		if(element->get_next()==_end){
		  if(element->get_next()->isInst() || 
		     element->get_next()->isDirective())
		    monflux<<"\t";
		  monflux << element->get_next()->get_content()<<endl;
		  break;
		}
		else element = element->get_next();
	
	      }
	    monflux<<"End\n\n"<<endl;
			
	  }
	
	  else {
	    cout<<"Error cannot open the file"<<endl;
	  }
	
	  monflux.close();
	}
	
	void Function::comput_label(){
	  Line* element = _head;
	
	  if(element == _end && element->isLabel())	
	    _list_lab.push_back(getLabel(element));
	  while(element != _end)
	    {
	
	      if(element->isLabel())	
		_list_lab.push_back(getLabel(element));
	
	      if(element->get_next()==_end){
		if(element->isLabel())	
		  _list_lab.push_back(getLabel(element));
		break;
	      }
	      else element = element->get_next();
	
	    }
	
	}
	
	int Function::nbr_label(){
	  return _list_lab.size();
	
	}
	
	Label* Function::get_label(int index){
	
	  list<Label*>::iterator it;
	  it=_list_lab.begin();
	
	  int size=(int) _list_lab.size();
	  if(index< size){
	    for (int i=0; i<index;i++ ) it++;
	    return *it;	
	  }
	  else cout<<"Error get_label : index is bigger than the size of the list"<<endl; 
		
	  return _list_lab.back();
	}
	
	Basic_block *Function::find_label_BB(OPLabel* label){
	  //Basic_block *BB = new Basic_block();
	   int size=(int)_myBB.size();
	   string str;
	   for(int i=0; i<size; i++){		
	      if(get_BB(i)->is_labeled()){
		 
		 str=get_BB(i)->get_head()->get_content();
		 if(!str.compare(0, (str.size()-1),label->get_op())){
		    return get_BB(i);
		 }
	      }
	  }
	  return NULL;
	}
	
	
	/* ajoute nouveau BB à la liste de BB de la fonction en le creant */
	
	void Function::add_BB(Line *debut, Line* fin, Line *br, int index){
	   Basic_block *b=new Basic_block();
	   b->set_head(debut);
	   b->set_end(fin);
	   b->set_index(index);
	   b->set_branch(br);
	   _myBB.push_back(b);
	}
	
	
	// //Calcule la liste des blocs de base : il faut délimiter les BB, en parcourant la liste des lignes/instructions
	// à partir de la premiere, il faut s'arreter à chaque branchement (et prendre en compte le delayed slot qui appartient
	//  au meme BB, c'est l'instruction qui suit tout branchement) ou à chaque label (on estime que tout label est utilisé par
	//  un saut et donc correspond bien à une entete de BB).
	
	void Function::comput_basic_block(){
	    Line *debut, *current, *prev;
	    bool verbose = true; 
	   current=_head;
	   debut=_head;
	   prev = NULL;
	   int ind=0;
	   Line *l=NULL;
	   Instruction *i=NULL;
	   Line * b;
	   cout<< "comput BB" <<endl;
	   if (verbose){
	   cout<<"head :"<<_head->get_content()<<endl;
	   cout<<"tail :"<<_end->get_content()<<endl;
	   }
	    Line *termin;
	   if (BB_computed) return; // NE PAS TOUCHER
	 while(current->isDirective())
	   current=current->get_next();
	   debut=current;
	   
	
	   while(current != _end->get_next()){ //traiter la derniere ligne donc s'arrêter à la suivante!
	    
	
	         i=dynamic_cast<Instruction*> (current);
		 if(i)
		   {
		     if(i->is_branch())
		       {
	               	 add_BB(debut,current->get_next(),current,ind++);
			 
			        //debut=current->get_next()->get_next();
			        current=current->get_next()->get_next();
			        debut=current;
			        continue;
		       }
		   }
	
		 else if(current->isLabel()&& current!=debut)
		   {
			
	             add_BB(debut,current->get_prev(),NULL,ind++);
		      	 debut=current;
		   }
		   if(!current->isDirective())
	            termin=current;
		 current=current->get_next();
	
	   }     //utile dans le cas ou la fonction ne termine pas par un branchement
	         if(current!=debut&&!debut->isDirective())
	       add_BB(debut,termin,NULL,ind++);
	   
	   if (verbose)
	     cout<<"end comput Basic Block"<<endl;
	   
	   BB_computed = true;
	   return;
	}
	
	int Function::nbr_BB(){
	   return _myBB.size();
	}
	
	Basic_block *Function::get_BB(int index){
	
	  list<Basic_block*>::iterator it;
	  it=_myBB.begin();
	  int size=(int)_myBB.size();
	
	  if(index< size){
	    for (int i=0; i<index;i++ ) it++;
	    return *it;	
	  }
	  else 
	    return NULL;
	}
	
	list<Basic_block*>::iterator Function::bb_list_begin(){
	   return _myBB.begin();
	}
	
	list<Basic_block*>::iterator Function::bb_list_end(){
	   return _myBB.end();
	}
	
	/* comput_pred_succ calcule les successeurs et prédécesseur des BB, pour cela il faut commencer par les successeurs */
	/* et itérer sur tous les BB d'une fonction */
	/* il faut determiner si un BB a un ou deux successeurs : dépend de la présence d'un saut présent ou non à la fin */
	/* pas de saut ou saut incontionnel ou appel de fonction : 1 successeur (lequel ?)*/
	/* branchement conditionnel : 2 successeurs */ 
	/* le dernier bloc n'a pas de successeurs - celui qui se termine par jr R31 */
	/* les sauts indirects n'ont pas de successeur */
	
	
	void Function::comput_succ_pred_BB(){
  
   list<Basic_block*>::iterator it, it2;
   Basic_block *current;
   Instruction *instr;
   Basic_block *succ=NULL;
   //alrady called in the main
   comput_label();
   int index = 0;
   if (BB_pred_succ) return;
   while(index < _myBB.size()){ 
    current = get_BB(index++);
    Line * temp = current->get_branch();
    if(temp == NULL){
		succ = get_BB(index);
      if(succ!= NULL)
        current->set_link_succ_pred(succ);
    }else{
      //TODO
      instr = getInst(temp);
      if(instr->is_branch ()){
        if (instr-> is_call()){
			succ = get_BB(index);
          if(succ != NULL)
            current->set_link_succ_pred(succ);
        }else if(!instr->is_indirect_branch()){
          current->set_link_succ_pred(find_label_BB(instr->get_op_label()));
          if(instr->is_cond_branch()){
            if((succ = get_BB(index)) != NULL)
              current->set_link_succ_pred(succ);
          }
        }
      }
    }
  }
   // ne pas toucher ci-dessous
   BB_pred_succ = true;
   return;
}

	void Function::compute_dom(){
	 list<Basic_block*>::iterator it, it2;
	  list<Basic_block*> workinglist;
	  Basic_block *current, *bb, *pred;
	  bool change = true;
	  int size = _myBB.size();
	  if (dom_computed) return;
	  comput_succ_pred_BB();
	 
	   it=_myBB.begin();	  
	   
	 for(int i=0;i<size;i++)
	 {
		 current=*it;
		 if(current->get_nb_pred()==0){
			  workinglist.push_back(current);
			 cout << "AJOUT DU BB nUMERO" << current->get_index() << endl;
			  current -> display();
		     for(int j=0;j<size;j++)
		        current->Domin[j]=false; 
		        break;
		 }
		
		 it++;
	 }
	 
	 
	while(!workinglist.empty()){
		change=false;	
		Basic_block* n= workinglist.front();
		
		
		if (n == NULL) cout << "bloc vide " << endl;
	
		workinglist.pop_front();
		cout << "affichage bloc traité " << endl;
		n -> display();
		
		vector<bool> t(NB_MAX_BB, true);
		
		for(int i=0;i<size; i++)
	     {
		     t[i] = n->Domin[i];
	     }
	   
	     
		for(int i=0;i<n->get_nb_pred();i++)
		{
			 pred=n->get_predecessor(i);
			 
			 for(int j=0;j<size;j++)
			   t[j]=t[j]&&pred->Domin[j];	  
		}
		
		 t[n->get_index()]=true;	
		 
		for(int i=0;i<size;i++)
	     {
		     if(t[i]!=n->Domin[i]){
				 change=true;
				 n->Domin=t;
				 break;
			 }
		       
	     }
		   
		 if(change){
			 if(n->get_nb_succ()!=0){
				 Basic_block* gg=n->get_successor1();
				 workinglist.push_back(gg);
				 cout << "ajout à WL du bloc numero " << gg -> get_index() << endl;
				 if(n->get_nb_succ()==2)
	               {
					   cout << "ajout à WL du bloc numero " << n->get_successor2()-> get_index() << endl;
					   workinglist.push_back(n->get_successor2());
				   }
			   }
		 }
		
	}
	
	  /* A REMPLIR */
	
	
	  // affichage du resultat
	  it=_myBB.begin();
	  
	  for (int j=0; j< size; j++){
	    current = *it;
	    cout << "Dominants pour BB" << current -> get_index() << " : "; 
	    for (int i = 0; i< nbr_BB(); i++){
	      if (current->Domin[i]) cout << " BB" << i  ;
	    }
	    cout << endl;
	    it++;
	  }
	  dom_computed = true;
	  return;
}
	
	void Function::compute_live_var(){
	  list<Basic_block*>::iterator it, it2;
	  list<Basic_block*> workinglist;
	  Basic_block *current, *bb, *pred;
	  bool change = true;
	  int size= (int) _myBB.size();
	  it=_myBB.begin();
		  
      for (int j=0; j<size; j++){	
		  current=*it;
		  if(current->get_nb_succ()==0){
			  workinglist.push_back(current);
			  
			  current->LiveOut[2]=true;
			  current->LiveOut[29]=true;
			  break;
		  }
		  it++;
	  }
	  
	  while(!workinglist.empty()){
		  change=false;	
		  Basic_block* n= workinglist.front();
		   n->compute_use_def();
		     vector<bool> lout(NB_REG,true);;
		      vector<bool> t(NB_REG,true);
		     lout=n->LiveOut;
		  	workinglist.pop_front();
		  	
		  	int compteur=0;
			while(true){// utilelors qu'un block se succede lui meme
				compteur++;	  	
				  	  if(n->get_nb_succ()!=0){
					   bb=n->get_successor1();
					   if(bb)
					   for (int j=0; j<NB_REG; j++)	
				           n->LiveOut[j] =bb->LiveIn[j];
				       bb=n->get_successor2();
		               if(bb)
		                 for (int j=0; j<NB_REG; j++)	
				           n->LiveOut[j]= n->LiveOut[j] || bb->LiveIn[j];
					  
				  }
				  	
				  	 for (int j=0; j<NB_REG; j++){	
						 if(n->Def[j])
						     t[j]=false;
						     else
				         t[j] =n->LiveOut[j];
				    }
				    
				  	 for (int j=0; j<NB_REG; j++){	
				         lout[j] =n->LiveIn[j];
				    }
				    
				  for (int j=0; j<NB_REG; j++){	
				      n->LiveIn[j]=n->Use[j]||t[j];
				  }
			if((n->get_successor1()==n||n->get_successor2()==n)&&compteur <2){
				continue;
			} else break;
		}
		
		  for (int j=0; j<NB_REG; j++){	
			    if(lout[j]!=n->LiveIn[j]){
		          change=true;
		          break;
			    }
  		  }
  		  
		if(change)
		   for(int j=0;j<n->get_nb_pred();j++){
			  workinglist.push_back(n->get_predecessor(j));
		     }
	     
		 
	  }
		  
	  // Affichage du resultat
	  it2=_myBB.begin();
	  for (int j=0; j<size; j++){
	    bb = *it2;
	    cout << "********* bb " << bb->get_index() << "***********" << endl;
	    cout << "LIVE_OUT : " ;
	    for(int i=0; i<NB_REG; i++){
	      if (bb->LiveOut[i]){
		cout << "$"<< i << " "; 
	      }
	    }
	    cout << endl;
	    cout << "LIVE_IN :  " ;
	    for(int i=0; i<NB_REG; i++){
	      if (bb->LiveIn[i]){
		cout << "$"<< i << " "; 
	      }}
	    cout << endl;
	    it2++;
	  }
	  return;
	}
	     
	
	
	
	/* en implementant la fonction test de la classe BB, permet de tester des choses sur tous les blocs de base d'une fonction par exemple l'affichage de tous les BB d'une fonction ou l'affichage des succ/pred des BBs comme c'est le cas -- voir la classe Basic_block et la méthode test */
	
	void Function::test(){
	  int size=(int)_myBB.size();
	   for(int i=0;i<size; i++){
	    get_BB(i)->test();
	  }
	   return;
	}
	
