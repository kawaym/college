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
    degree: u32,
}

impl Vertex {
    fn new(id: String) -> Self {
        Vertex {
            id,
            edges: Vec::new(),
            degree: 0,
        }
    }

    fn add_degree(&mut self) {
        self.degree = self.degree + 1
    }

    fn add_edge(&mut self, target: String, weight: i32) {
        self.add_degree();
        self.edges.push(Edge { target, weight })
    }
}

#[derive(Debug)]
enum GraphKind {
    Bidirectional,
    Unidirectional,
}

#[derive(Debug)]
pub struct Graph {
    vertices: HashMap<String, Vertex>,
    kind: GraphKind,
}

impl Graph {
    pub fn new() -> Self {
        Graph {
            vertices: HashMap::new(),
            kind: GraphKind::Bidirectional,
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

    pub fn get_vertices_number(&self) -> u32 {
        self.vertices.len() as u32
    }

    pub fn get_edges_number(&self) -> u32 {
        let mut number = 0;
        for (_, vertex) in &self.vertices {
            number += vertex.degree
        }

        match self.kind {
            GraphKind::Bidirectional => number = number / 2,
            GraphKind::Unidirectional => number = number,
        }

        return number;
    }

    pub fn get_degree_minimum(&self) -> u32 {
        let mut number: u32 = u32::max_value();
        for (_, vertex) in &self.vertices {
            if vertex.degree < number {
                number = vertex.degree
            }
        }
        number
    }

    pub fn get_degree_maximum(&self) -> u32 {
        let mut number: u32 = 0;
        for (_, vertex) in &self.vertices {
            if vertex.degree > number {
                number = vertex.degree
            }
        }
        number
    }

    pub fn get_degree_average(&self) -> f32 {
        let mut number = 0.0;
        for (_, vertex) in &self.vertices {
            number += vertex.degree as f32
        }
        number /= self.vertices.len() as f32;
        number
    }
}
