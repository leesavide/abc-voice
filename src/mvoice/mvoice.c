/* mvoice.c
 * Splits raw audio files and microphone input into voices, in preparation to
 * using abcv to analyze the notation within each voice. This said, the quality
 * of the music notation is relative to the quality of the audio in question.
 * Additionally, note that this will NOT constitute as a means for accurate
 * music notation, no matter how accurate it may be. In the end, music notation
 * has to be interpretted by humans, by ear, to be fully accurate. These tools
 * only serve to make that process a great deal easier.
 * 
 * Lee Savide <laughingman182@gmail.com> (C) 2012. All rights reserved.
 * Please read the license, notice, and readme.
 */

