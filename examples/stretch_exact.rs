use std::fs::File;
use std::path::PathBuf;

use anyhow::bail;
use clap::Parser;
use cpal::FromSample;

#[derive(Parser, Debug)]
#[command(name = "stretch")]
struct Args {
    /// The file to stretch
    input_file: PathBuf,
    /// The output file
    output_file: PathBuf,
    /// The time stretch factor.
    #[arg(long, default_value = "1")]
    rate: f32,
    /// The pitch adjustment, in semitones.
    #[arg(long, default_value = "0")]
    semitones: f32,
}

fn main() -> anyhow::Result<()> {
    let args = Args::parse();

    let file = File::open(args.input_file)?;
    let wav_reader = hound::WavReader::new(file)?;
    let spec = wav_reader.spec();

    let output = match (spec.bits_per_sample, spec.sample_format) {
        (16, hound::SampleFormat::Int) => {
            stretch_exact::<i16, _>(wav_reader, args.rate, args.semitones)
        }
        (32, hound::SampleFormat::Float) => {
            stretch_exact::<f32, _>(wav_reader, args.rate, args.semitones)
        }
        (depth, format) => bail!("unsupported sample format: {depth}bit {format:?}"),
    }?;

    let mut wav_writer = hound::WavWriter::create(args.output_file, spec)?;

    for sample in output {
        wav_writer.write_sample(sample)?;
    }

    Ok(())
}

fn stretch_exact<T, R>(
    input_file: hound::WavReader<R>,
    rate: f32,
    semitones: f32,
) -> anyhow::Result<Vec<f32>>
where
    T: hound::Sample + Send + 'static,
    R: std::io::Read + Send + 'static,
    f32: FromSample<T>,
{
    let spec = input_file.spec();

    let output_len = (input_file.len() as f32 / rate) as usize;

    let mut output = vec![0.0f32; output_len];

    let samples: Vec<_> = input_file
        .into_samples::<T>()
        .filter_map(Result::ok)
        .map(f32::from_sample_)
        .collect();
    let mut stretch =
        signalsmith_stretch::Stretch::preset_default(spec.channels as u32, spec.sample_rate);
    stretch.set_transpose_factor_semitones(semitones, None);

    stretch.exact(samples, &mut output);

    Ok(output)
}
