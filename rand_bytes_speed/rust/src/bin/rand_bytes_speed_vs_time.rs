use std::fs::{File, OpenOptions};
use std::io::{self, Read, Write};
use std::time::{Instant, SystemTime};
use std::env;

const READ_SIZE: usize = 1024;

fn main() -> io::Result<()> {
    let args: Vec<String> = env::args().collect();
    if args.len() != 4 {
        println!("Usage: {} source step_duration total_duration", args[0]);
        return Ok(());
    }

    let source = &args[1];
    let step_duration = args[2].parse::<u64>().expect("step_duration must be an integer");
    let total_duration = args[3].parse::<u64>().expect("total_duration must be an integer");

    let steps = total_duration / step_duration;

    let mut random_data = File::open(source)?;

    let mut buffer = vec![0; READ_SIZE];
    let mut mb_per_sec = vec![0.0; steps as usize];

    for i in 0..steps {
        let step_start = Instant::now();
        let mut total_bytes_read: usize = 0;
        while step_start.elapsed().as_secs() < step_duration {
            let bytes_read = random_data.read(&mut buffer)?;
            total_bytes_read += bytes_read;
        }

        mb_per_sec[i as usize] = total_bytes_read as f64 / (1024.0 * 1024.0) / step_duration as f64;
    }

    let csv_file_name = format!("results_{}.csv", SystemTime::now().duration_since(SystemTime::UNIX_EPOCH).unwrap().as_secs());
    let mut csv_file = OpenOptions::new().write(true).create(true).open(&csv_file_name)?;
    
    write!(csv_file, "step,rate\n")?;
    for i in 0..steps {
        write!(csv_file, "{},{}\n", (i + 1) * step_duration, mb_per_sec[i as usize])?;
    }

    println!("Results saved to {}", csv_file_name);

    Ok(())
}