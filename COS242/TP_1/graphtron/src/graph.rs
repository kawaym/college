use std::collections::{HashMap, HashSet, VecDeque};

#[derive(Debug)]
struct Edge {
    target: String,
    weight: i32,
}

#[derive(Debug)]
enum VertexStatus {
    Marked,
    Unmarked,
}
#[derive(Debug)]
pub struct Vertex {
    id: String,
    edges: Vec<Edge>,
    degree: u32,
    status: VertexStatus,
}

impl Vertex {
    fn new(id: String) -> Self {
        Vertex {
            id,
            edges: Vec::new(),
            degree: 0,
            status: VertexStatus::Unmarked,
        }
    }

    fn add_degree(&mut self) {
        self.degree = self.degree + 1
    }

    fn add_edge(&mut self, target: String, weight: i32) {
        self.add_degree();
        self.edges.push(Edge { target, weight })
    }

    fn mark(&mut self) {
        self.status = VertexStatus::Marked
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

    pub fn display(&self, format: String) {
        if format == "Matriz" {
            self.display_adjacency_matrix();
            return;
        }

        if format == "Lista" {
            self.display_adjacency_list();
            return;
        }

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

    pub fn get_degree_median(&self) -> f32 {
        let mut vertices: Vec<u32> = vec![];
        let mut number: f32 = -1.0;
        for (_, vertex) in &self.vertices {
            vertices.push(vertex.degree.clone());
        }
        vertices.sort_by(|a, b| a.partial_cmp(b).unwrap());

        if vertices.len() % 2 != 0 {
            number = vertices[vertices.len() / 2] as f32
        } else {
            number = (vertices[vertices.len() / 2] as f32
                + vertices[vertices.len() / 2 - 1] as f32)
                / 2.0;
        }
        number
    }

    fn create_adjacency_matrix(&self) -> Vec<Vec<i32>> {
        let mut matrix = vec![vec![0; self.vertices.len()]; self.vertices.len()];

        let vertex_indices: HashMap<String, usize> = self
            .vertices
            .keys()
            .enumerate()
            .map(|(i, v)| (v.clone(), i))
            .collect();

        for (from, vertex) in &self.vertices {
            if let Some(&i) = vertex_indices.get(from) {
                for edge in &vertex.edges {
                    if let Some(&j) = vertex_indices.get(&edge.target) {
                        matrix[i][j] = edge.weight;
                    }
                }
            }
        }

        matrix
    }

    fn display_adjacency_matrix(&self) {
        let matrix = &self.create_adjacency_matrix();
        let vertices = &self.vertices.keys().cloned().collect::<Vec<String>>();

        println!("{:?}", vertices);
        println!("{:?}", matrix);
    }

    fn create_adjacency_list(&self) -> HashMap<String, Vec<(String, i32)>> {
        let mut list = HashMap::new();

        for (vertex_id, vertex) in &self.vertices {
            let edges: Vec<(String, i32)> = vertex
                .edges
                .iter()
                .map(|edge| (edge.target.clone(), edge.weight))
                .collect();
            list.insert(vertex_id.clone(), edges);
        }

        list
    }

    fn display_adjacency_list(&self) {
        let list = &self.create_adjacency_list();

        println!("{:?}", list)
    }

    fn umark_all_vertices(&mut self) {
        for (_, vertex) in &mut self.vertices {
            vertex.status = VertexStatus::Unmarked
        }
    }

    pub fn bfs(&mut self, root_id: &str) -> Vec<String> {
        let mut visited = HashSet::new();
        let mut queue = VecDeque::new();
        let mut result = Vec::new();

        // Verifica se o vértice inicial existe
        if self.vertices.contains_key(root_id) {
            queue.push_front(root_id.to_string());
            visited.insert(root_id.to_string());

            loop {
                let vertex_id = queue.remove(0);
                result.push(vertex_id.clone());

                if let Some(vertex) = self.vertices.get(&vertex_id) {
                    for edge in &vertex.edges {
                        if !visited.contains(&edge.target) {
                            visited.insert(edge.target.clone());
                            queue.push(edge.target.clone());
                        }
                    }
                }
            }
        }

        result
    }
}
