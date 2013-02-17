#ifndef GRAPH_H
#define GRAPH_H

// if you want the XML result of the graph able to be opened by IE / Chrome, uncomment this
#define GRAPHXML_DISPLAY_ENCODE

///////////////////////////////////////////////////////////////
// Graph.h - Graph Library                                   //
// Ver 1.2                                                   //
// Language:    Visual C++ 2012                              //
// Platform:    Lenova ThinkPad E420s, Windows 7             //
// Application: Help for CSE687 - OOD, Pr#1, Spring 2013     //
// Author:      Jim Fawcett, CST 4-187, Syracuse University  //
//              (315) 443-3948, jfawcett@twcny.rr.com        //
///////////////////////////////////////////////////////////////
/*
Package Operations:
===================
A graph is a collection of vertices, connected by edges.  Each
vertex holds some kind of information held in an instance of
a type V and an integer id, which should be unique with a given
graph.  Each edge connects a parent vertex with a child
vertex and it too holds information in an instance of a type E.

This package provides two classes that support creation of
instances of a graph: Vertex<V,E> and Graph<V,E>.  These are
template-based with parameters V and E, as described above.

The graph holds a composed collection of vertices.  Each 
vertex may hold one or more edges.  The edge is implemented
as an instance of std::pair<int,E> where the first parameter
is the id of the child vertex and the second parameter is
an instance of the edge information type E.

Note:
It is important that vertex ids be unique.  If you set any of
them with the second argument of the Vertex constructor or with
the id() function it is up to you to ensure uniqueness.  If you
don't explicitly set any ids then the constructor insures that
vertices get sequential ids in the order constructed.

Maintanence Information:
========================
Required files:
---------------
Graph.h, Graph.cpp, XmlReader.h, XmlWriter.h, StringHelper.h

Build Process:
--------------
Using Visual Studio Command Prompt:
devenv Graph.sln /rebuild debug

Revision History:
-----------------
- ver 1.3 : changed by Kevin Wang for Pr#1, add ability of parsing XML, reading/writing Graph from/to XML
- ver 1.2 : changed id() return to reference so can be changed
- ver 1.1 : added prologue comments, compile test directive
- ver 1.0 : first release

*/
/*
* To Do:
* - Strong Components, Topological Sorting
* - Find disconnected graph partitions
*/

#include <vector>
#include <unordered_map>
#include "XmlWriter.h"
#include "XmlReader.h"
#include "StringHelper.h"

namespace GraphLib 
{
	///////////////////////////////////////////////////////////////
	// Functor class declaration
	template<typename V, typename E> class showVertex;

	///////////////////////////////////////////////////////////////
	// Vertex class

	template<typename V, typename E> 
	class Vertex
	{
	public:
		typedef std::pair<int, E> Edge;  // graph index of target id, edge type	!!> should be changed!
		typename typedef std::vector<Edge>::iterator iterator;
		iterator begin();
		iterator end();
		Vertex(V v, int id=-1);
		void add(Edge& edge);
		// Vertex(const Vertex<V,E>& v);                  compiler generated is correct
		// Vertex<V,E>& operator=(const Vertex<V,E>& v);  compiler generated is correct
		Edge& operator[](size_t i);
		Edge operator[](size_t i) const;
		V& value();
		size_t& id(); 
		size_t size();
		bool& mark();

		///////////////////////////////////////////////////////
		// Change Vertex from here
		inline std::string xml();
		std::string xml_noroot();
		void xml(const std::string& str);	// read from xml string
		inline void reset();
		std::vector<std::pair<int, E>>& edges();
		Vertex() : _mark(false){};	// make a constructor with no parameter
		// Change end
		///////////////////////////////////////////////////////
	private:
		std::vector<Edge> _edges;
		V _v;
		size_t _id;
		static size_t count;
		bool _mark;
	};
	//----< reserve memory for, and initialize, static count >-----
	template<typename V, typename E>
	size_t Vertex<V,E>::count = 0;

	//----< set and return boolean mark, used for traversal >------
	template<typename V, typename E>
	bool& Vertex<V,E>::mark() { return _mark; }

	//----< return iterator pointing to first edge >---------------
	template<typename V, typename E>
	typename Vertex<V,E>::iterator Vertex<V,E>::begin() { return _edges.begin(); }

	//----< return iterator pointing to one past last edge >-------
	template<typename V, typename E>
	typename Vertex<V,E>::iterator Vertex<V,E>::end() { return _edges.end(); }

	//----< construct instance >-----------------------------------
	template<typename V, typename E>
	Vertex<V,E>::Vertex(V v, int id) : _v(v), _id(count++), _mark(false) 
	{
		if(id != -1)
			_id = id;
	}

	//----< add edge to vertex edge collection >-------------------
	template<typename V, typename E>
	void Vertex<V,E>::add(Edge& edge) { _edges.push_back(edge); }

	//----< index non-const vertex's edges >-----------------------
	template<typename V, typename E>
	typename Vertex<V,E>::Edge& Vertex<V,E>::operator[](size_t i) { return _edges[i]; }

	//----< index const vertex's edges >---------------------------
	template<typename V, typename E>
	typename Vertex<V,E>::Edge Vertex<V,E>::operator[](size_t i) const { return _edges[i]; }

	//----< set and read value of vertex's held type, V >----------
	template<typename V, typename E>
	V& Vertex<V,E>::value() { return _v; }

	//----< return vertex's id >-----------------------------------
	template<typename V, typename E>
	size_t& Vertex<V,E>::id() { return _id; }

	//----< return number of edges >-------------------------------
	template<typename V, typename E>
	size_t Vertex<V,E>::size() { return _edges.size(); }

	///////////////////////////////////////////////////////////////
	// Graph class

	template<typename V, typename E>
	class Graph
	{
	public:
		typename typedef std::vector< Vertex<V,E> >::iterator iterator;
		iterator begin();
		iterator end();
		// Graph(const Graph<V,E>& g);                  compiler generated is correct
		// Graph<V,E>& operator=(const Graph<V,E>& g);  compiler generated is correct
		Vertex<V,E>& operator[](size_t i);
		Vertex<V,E> operator[](size_t i) const;
		void addVertex(Vertex<V,E> v);
		void addEdge(E eval, Vertex<V,E>& parent, Vertex<V,E>& child);
		size_t findVertexIndexById(size_t id);
		size_t size();
		void dfs(Vertex<V,E>& v, void(*f)(Vertex<V,E>& v));

		///////////////////////////////////////////////////////
		// Change graph from here
		size_t addOrFindVertexIndexByValue(const V& vVal);
		size_t findVertexIndexByValue(const V& vVal);
		std::vector<Vertex<V,E>> findVertexesByValue(const V& vVal);
		std::vector<std::pair<int, E>> findEdgesByValue(const E& eVal);
		void tryAddEdge(const E& eVal, const V& parent, const V& child);
		inline std::string xml();
		void xml(const std::string& str);	// read from xml string
		std::string xml_noroot();
		inline void reset();
		void clean();
		void addVertexByValue(const V& vVal);
		// for functor
		void dfs(Vertex<V,E>& v, showVertex<V,E>& f);
		void dfsCore(Vertex<V,E>& v, showVertex<V,E>& f);
		// Change end
		///////////////////////////////////////////////////////
	private:
		std::vector< Vertex<V,E> > adj;
		std::unordered_map<size_t, size_t> idMap; // id maps to graph index
		void dfsCore(Vertex<V,E>& v, void(*f)(Vertex<V,E>& v));

		///////////////////////////////////////////////////////
		// Change graph from here
		std::string _xml;
		// Change end
		///////////////////////////////////////////////////////

	};
	//----< return iterator pointing to first vertex >-------------
	template<typename V, typename E>
	typename Graph<V,E>::iterator Graph<V,E>::begin() { return adj.begin(); }

	//----< return iterator pointing one past last vertex >--------
	template<typename V, typename E>
	typename Graph<V,E>::iterator Graph<V,E>::end() { return adj.end(); }

	//----< index non-const graph's vertex collection >------------
	template<typename V, typename E>
	typename Vertex<V,E>& Graph<V,E>::operator[](size_t i) { return adj[i]; }

	//----< index const graph's vertex collection >----------------
	template<typename V, typename E>
	typename Vertex<V,E> Graph<V,E>::operator[](size_t i) const { return adj[i]; }

	//----< add vertex to graph's vertex collection >--------------
	template<typename V, typename E>
	void Graph<V,E>::addVertex(Vertex<V,E> v) 
	{ 
		adj.push_back(v);
		idMap[v.id()] = adj.size() - 1;
	}
	//----< return number of vertices in graph's collection >------
	template<typename V, typename E>
	size_t Graph<V,E>::size() { return adj.size(); }

	//----< return index of vertex with specified id >-------------
	template<typename V, typename E>
	size_t Graph<V,E>::findVertexIndexById(size_t id)
	{
		return idMap[id];
	}
	//----< add edge from specified parent to child vertices >-----
	template<typename V, typename E>
	void Graph<V,E>::addEdge(E eVal, Vertex<V,E>& parent, Vertex<V,E>& child)
	{
		size_t childIndex = findVertexIndexById(child.id());
		if(childIndex == adj.size())
			throw std::exception("no edge child");
		size_t parentIndex = findVertexIndexById(parent.id());
		if(parentIndex == adj.size())
			throw std::exception("no edge parent");
		Vertex<V,E>::Edge e;
		e.first = child.id();	// changed to child id - Kevin
		e.second = eVal;
		adj[parentIndex].add(e);
	}
	//----< recursive depth first search with action f >-----------
	template<typename V, typename E>
	void Graph<V,E>::dfsCore(Vertex<V,E>& v, void(*f)(Vertex<V,E>& cv))
	{
		f(v);
		v.mark() = true;
		for(auto edge : v)
		{
			size_t index = findVertexIndexById(e.first);
			if (index<adj.size() && adj[index].mark() == false)
				dfsCore(adj[index], f);
		}
		for(auto& vert : adj)
		{
			if(vert.mark() == false)
				dfsCore(vert, f);
		}
	}
	//----< depth first search, clears marks for next search >-----
	template<typename V, typename E>
	void Graph<V,E>::dfs(Vertex<V,E>& v, void(*f)(Vertex<V,E>& cv))
	{
		for(auto& vert : adj)
			vert.mark() = false;
		dfsCore(v, f);
		for(auto& vert : adj)
			vert.mark() = false;
	}

	///////////////////////////////////////////////////////////////
	// functor implementation!!
	template<typename V, typename E>
	class showVertex {
	public:
		//----< functor body, display vertex infor >-----
		inline void operator()(Vertex<V,E>& v) {
			std::cout << "\nvertex id = " << v.id() << ", value = " << (v.value()).c_str();
		}
	};

	///////////////////////////////////////////////////////////////
	// Graph class extends here

	//----< overloaded recursive depth first search for functor >-----------
	template<typename V, typename E>
	void Graph<V,E>::dfsCore(Vertex<V,E>& v, showVertex<V,E>& f)
	{
		f(v);
		v.mark() = true;
		for(auto edge : v)
		{
			size_t index = findVertexIndexById(edge.first);
			if(index<adj.size() && adj[index].mark() == false)
				dfsCore(adj[index], f);
		}
		for(auto& vert : adj)
		{
			if(vert.mark() == false)
				dfsCore(vert, f);
		}
	}

	//----< depth first search, clears marks for next search >-----
	template<typename V, typename E>
	void Graph<V,E>::dfs(Vertex<V,E>& v, showVertex<V,E>& f)
	{
		for(auto& vert : adj)
			vert.mark() = false;
		dfsCore(v, f);
		for(auto& vert : adj)
			vert.mark() = false;
	}
	
	//----< get XML content of one graph, without XML declaration and root element >-----
	template<typename V, typename E>
	std::string Graph<V,E>::xml_noroot() {
		std::string _xml="";
		for (iterator v = begin(); v!=end();v++)
			_xml += v->xml_noroot();
		return _xml;
	}
	
	//----< get XML contont of one graph >-----
	template<typename V, typename E>
	inline std::string Graph<V,E>::xml() {
		return Helper::xmlWrapper(xml_noroot(),"graph");
	}
	
	//----< try to add one edge. if one edge with same value exist, then exist >-----
	template<typename V, typename E>
	void Graph<V,E>::tryAddEdge(const E& eVal, const V& parent, const V& child)
	{
		size_t pIndex = addOrFindVertexIndexByValue(parent), cIndex = addOrFindVertexIndexByValue(child);
		Vertex<V,E>& p = adj[pIndex], c=adj[cIndex];
		// check if there is a edge with the same eVal exist
		for (auto iter = p.begin() ; iter != p.end() ; iter++ )
			if (iter->first == c.id() && iter->second == eVal)
				return;
		// no luck, didn't find, then add one
		Vertex<V,E>::Edge e;
		e.first = c.id();
		e.second = eVal;
		p.add(e);
	}

	// return the first vertex index whose value is as specificed
	template<typename V, typename E>
	size_t Graph<V,E>::findVertexIndexByValue(const V& vVal)
	{
		for (size_t i=0 ; i<size(); i++)
			if (adj[i].value() == vVal)
				return i;
		// return size() when not found
		return size();
	}
	
	//----< find a vertex with one specific value, if not found, create one >-----
	template<typename V, typename E>
	size_t Graph<V,E>::addOrFindVertexIndexByValue(const V& vVal)
	{
		size_t index = findVertexIndexByValue(vVal);
		if (index<size())	// found
			return index;
		// do not find the vertex, try add one
		Vertex<V,E> v(vVal);
		v.mark()=false;	// this mark makes it a "informal" type
		addVertex(v);
		// now we need to return the copied version of vertex storied in graph
		return size()-1;	// index == size()-1
	}
	
	//----< add a vertex with specific value, and mark it >-----
	template<typename V, typename E>
	void Graph<V,E>::addVertexByValue(const V& vVal)
	{
		size_t index = findVertexIndexByValue(vVal);
		if (index<size()) {	// found
			adj[index].mark()=true;
		}
		else {
			// do not find the vertex, try add one
			Vertex<V,E> v(vVal);
			v.mark()=true;
			addVertex(v);
		}
	}
	
	//----< get XML of one vertex, without XML declaration and root element >-----
	template<typename V, typename E>
	std::string Vertex<V,E>::xml_noroot() {
		std::string _xml="";
		XmlWriter vwtr;	// vector writer
		std::ostringstream vId, vVal;
		vId << id();	// translate into string
		vVal << value();
		vwtr.start("vertex");
#ifdef GRAPHXML_DISPLAY_ENCODE
		vwtr.addAttribute("id", Helper::encodeXml(vId.str()));
		vwtr.addAttribute("value", Helper::encodeXml(vVal.str()));
#else
		vwtr.addAttribute("id", vId.str());
		vwtr.addAttribute("value", vVal.str());
#endif
		for(iterator e = begin(); e!=end();e++)
		{
			XmlWriter ewtr;	// edge writer
			std::ostringstream eTo, eType, eRefer;
			eTo << e->first;
			eType << e->second;
			ewtr.start("edge");
#ifdef GRAPHXML_DISPLAY_ENCODE
			ewtr.addAttribute("id", Helper::encodeXml(eTo.str()));
			ewtr.addAttribute("value", Helper::encodeXml(eType.str()));
#else
			ewtr.addAttribute("id", eTo.str());
			ewtr.addAttribute("value", eType.str());
#endif
			ewtr.end();
			vwtr.addBody(ewtr.xml());
		}
		vwtr.end();
		_xml += vwtr.xml();
		return _xml;
	}
	
	//----< return XML of one vertex >-----
	template<typename V, typename E>
	inline std::string Vertex<V,E>::xml() {
		return Helper::xmlWrapper(xml_noroot(),"vertexes");
	}

	// delete all vertex whose mark is false
	template<typename V, typename E>
	void Graph<V,E>::clean() {
		std::vector<size_t> deletedV;
		// delete all false marked vertexes
		for (iterator it=begin();it!=end();) {
			if (!it->mark()) {
				deletedV.push_back(it->id());
				it = adj.erase(it);
			}
			else it++;
		}
		// the second step, delete all related edges
		for (iterator it=begin();it!=end();it++) {
			it->edges().erase(
				std::remove_if(it->edges().begin(), it->edges().end(), [deletedV](const std::pair<int, E> & e) {
					bool isFound = false;
					for (size_t i=0;i<deletedV.size();i++)
						if (deletedV[i]==e.first)
							return true;
					return false;
			}), it->edges().end());
		}
	}
	
	//----< reset this vertex, delete all edges >-----
	template<typename V, typename E>
	inline void Vertex<V,E>::reset() {
		_edges.clear();
		_mark=false;
	}
	
	//----< reset this graph, delete all vertexes >-----
	template<typename V, typename E>
	inline void Graph<V,E>::reset() {
		adj.clear();
		idMap.clear();	// index and id relationship will be rebuilt
	}
	
	//----< find vertexes with one specific value >-----
	template<typename V, typename E>
	std::vector<Vertex<V,E>> Graph<V,E>::findVertexesByValue(const V& vVal) {
		std::vector<Vertex<V,E>> vertexes;
		for (size_t i=0 ; i<size(); i++)
			if (adj[i].value() == vVal)
				vertexes.push_back(adj[i]);
		return vertexes;
	}
	
	//----< find edges with one specific value >-----
	template<typename V, typename E>
	std::vector<std::pair<int, E>> Graph<V,E>::findEdgesByValue(const E& eVal) {
		std::vector<std::pair<int, E>> edges;
		for (iterator it=begin();it!=end();it++)
			for (size_t i=0 ; i<it->size(); i++)
				if ((*it)[i].second == eVal)
					edges.push_back((*it)[i]);
		return edges;
	}

	// read from xml string
	template<typename V, typename E>
	void Graph<V,E>::xml(const std::string& str) {
		XmlReader rdr(str);
		rdr.reset();
		reset();	// reset graph
		// find the "graph" root tag
		for (int i=0; i<2 && rdr.next();i++) ;
		if (rdr.tag()!="graph")	// return when the root tag is not "graph"
			return;
		while(rdr.next())	// notice!! the XML reader actually crash when parsing an attribute includes "<>", '\n' and space, I am just going to avoid using them, 'cus I am not gonna to debug it
		{
			if (rdr.tag()=="vertex") { // create vertexes
				Vertex<V,E> v;
				v.xml(rdr.element().c_str());
				addVertex(v);
			}
		}
	}
	
	//----< get all edges of one vertex >-----
	template<typename V, typename E>
	std::vector<std::pair<int, E>>& Vertex<V,E>::edges() {
		return _edges;
	}
	
	//----< read vertex information from one xml clip >-----
	template<typename V, typename E>
	void Vertex<V,E>::xml(const std::string& str) {	// read from xml string
		XmlReader rdr(str);
		rdr.reset();
		reset();
		if (rdr.next() && rdr.tag()!="vertex")
			return;
		XmlReader::attribElems vAttr = rdr.attributes();
		// load attributes
		for (size_t i=0; i<vAttr.size(); ++i) {
			if (vAttr[i].first=="id")
				_id = atoi(vAttr[i].second.c_str());
			else if (vAttr[i].first=="value")
				_v = vAttr[i].second;	// you should implement an operator for convertion from string to the V
		}
		// load edges
		while (rdr.next()) {
			if (rdr.tag()!="edge")
				continue;
			XmlReader::attribElems eAttr = rdr.attributes();
			std::pair<int, E> edge;
			for (size_t i=0; i<eAttr.size(); ++i) {
				if (eAttr[i].first=="id")
					edge.first = atoi(eAttr[i].second.c_str());
				else if (eAttr[i].first=="value")
					edge.second = eAttr[i].second;	// you should implement an operator for convertion from string to the V
			}
			add(edge);
		}
	}
	
	//----< a global function that creates a vertex from XML >-----
	template<typename V, typename E>
	static Vertex<V,E> createVertexFromXml(const std::string& str) {
		Vertex<V,E> v;
		v.xml(str);
		return v;
	}
	
	//----< a global function that creates a graph from XML >-----
	template<typename V, typename E>
	static Graph<V,E> createGraphFromXml(const std::string& str) {
		Graph<V,E> g;
		g.xml(str);
		return g;
	}

	// Change end here
	///////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////
	// Display class

	template<typename V, typename E>
	class Display
	{
	public:
		typedef Graph<V,E> graph;
		typedef Vertex<V,E> vertex;
		typedef std::pair<int,E> edge;

		static std::vector< Vertex<V,E> > vertsWithNoParents(graph& g)
		{
			std::vector<size_t> parentCount;
			graph::iterator iter = g.begin();
			while(iter != g.end())
			{
				parentCount.push_back(0);
				++iter;
			}
			iter = g.begin();
			while(iter != g.end())
			{
				vertex v = *iter;
				for(size_t i=0; i<v.size(); ++i)
				{
					edge e = v[i];
					size_t index = g.findVertexIndexById(e.first);
					if (index<g.size())
						parentCount[index]++;
				}
				++iter;
			}
			std::vector<vertex> noParents;
			for(size_t j=0; j<g.size(); ++j)
				if(parentCount[j] == 0)
					noParents.push_back(g[j]);
			return noParents;
		}

		static void show(graph& g)
		{
			graph::iterator iter = g.begin();
			while(iter != g.end())
			{
				vertex v = *iter;
				std::cout << "\n  vertex id = " << v.id() << ", value = " << (v.value()).c_str();
				for(size_t i=0; i<v.size(); ++i)
				{
					vertex::Edge edge = v[i];
					size_t index = g.findVertexIndexById(edge.first);
					if (index>=g.size())
						continue;
					std::cout << "\n    edge points to vertex with id = " << edge.first;
					std::cout << " and value = " << g[g.findVertexIndexById(edge.first)].value().c_str();
					std::cout << ", edge value = " << (edge.second).c_str();
				}
				++iter;
			}
		}
	};
}

#endif
