fn print_list(list: &Vec<Option<i32>>) {
    for number in list {
        match number {
            Some(num) => print!("{} ", num),
            None => print!("NaN "),
        }
    }
    println!("")
}

fn swap(left: usize, right: usize, list: &mut Vec<Option<i32>>) {
    if left == right {
        return;
    }
    let temp1 = list[left].take().unwrap();
    let temp2 = list[right].take().unwrap();
    list[left] = Some(temp2);
    list[right] = Some(temp1);
}

pub fn permute(left: usize, right: usize, list: &mut Vec<Option<i32>>) {
    if left == right {
        // print_list(list);
    } else {
        for i in left..right + 1 {
            swap(left, i, list);
            permute(left + 1, right, list);
            swap(left, i, list);
        }
    }
}
