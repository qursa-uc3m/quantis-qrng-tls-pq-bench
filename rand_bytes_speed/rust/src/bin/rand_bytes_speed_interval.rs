use std::fs::File;
use std::io::{Read, Error};
use std::time::{Instant, Duration};

const READ_SIZE: usize = 1024;
const DURATION: u64 = 10;

fn main() -> Result<(), Error> {
    let start_time = Instant::now();
    let mut total_bytes_read: usize = 0;
    let mut random_data = File::open("/dev/random")?;

    let mut buffer = [0; READ_SIZE];
    
    while start_time.elapsed() < Duration::from_secs(DURATION) {
        let bytes_read = random_data.read(&mut buffer)?;
        total_bytes_read += bytes_read;
    }

    let mb_per_sec = (total_bytes_read as f64) / (1024.0 * 1024.0) / (DURATION as f64);
    println!("Average rate: {:.4} MB/sec", mb_per_sec);
    
    Ok(())
}