#inlcude<vector>

namespace Network {


  class Edge;

  class Node
  {
    Node();
    
    ~Node();
    

  private:
    std::vector<std::weak_ptr<Edge>> starting_edges;
    std::vector<std::weak_ptr<Edge>> ending_edges;


  };

}
