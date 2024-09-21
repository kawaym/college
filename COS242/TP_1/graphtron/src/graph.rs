use std::collections::HashMap;

#[derive(Debug)]
struct Edge {
    target: String,
    weight: i32,
}

#[derive(Debug)]
pub struct Vertex {
    id: String,
    edges: Vec<Edge>,
}

impl Vertex {
    fn new(id: String) -> Self {
        Vertex {
            id,
            edges: Vec::new(),
        }
    }

    fn add_edge(&mut self, target: String, weight: i32) {
        self.edges.push(Edge { target, weight })
    }
}

#[derive(Debug)]
pub struct Graph {
    vertices: HashMap<String, Vertex>,
}

impl Graph {
    pub fn new() -> Self {
        Graph {
            vertices: HashMap::new(),
        }
    }

    pub fn add_vertex(&mut self, id: String) {
        self.vertices.entry(id.clone()).or_insert(Vertex::new(id));
    }

    pub fn add_edge(&mut self, from: String, to: String, weight: i32) {
        self.add_vertex(from.clone());
        self.add_vertex(to.clone());
        if let Some(vertex) = self.vertices.get_mut(&from) {
            vertex.add_edge(to.clone(), weight);
        } else {
            println!("Vertex {from} was not found!");
        }
        if let Some(vertex) = self.vertices.get_mut(&to) {
            vertex.add_edge(from.clone(), weight);
        } else {
            println!("Vertex {to} was not found!");
        }
    }

    pub fn display(&self) {
        for (id, vertex) in &self.vertices {
            println!("Vertex {id}:");
            for edge in &vertex.edges {
                println!(" -> {} (weight {})", edge.target, edge.weight)
            }
        }
    }

    pub fn get_vertices_number(self) -> usize {
        self.vertices.len()
    }
}
