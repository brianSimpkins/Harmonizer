if {[catch {

# define run engine funtion
source [file join {C:/lscc/radiant/3.2} scripts tcl flow run_engine.tcl]
# define global variables
global para
set para(gui_mode) 1
set para(prj_dir) "C:/Users/bsimpkins/Documents/Harmonizer-adc_brian/fpga_code"
# synthesize IPs
# synthesize VMs
# synthesize top design
file delete -force -- FFT_fft.vm FFT_fft.ldc
run_engine_newmsg synthesis -f "FFT_fft_lattice.synproj"
run_postsyn [list -a iCE40UP -p iCE40UP5K -t SG48 -sp High-Performance_1.2V -oc Industrial -top -w -o FFT_fft_syn.udb FFT_fft.vm] "C:/Users/bsimpkins/Documents/Harmonizer-adc_brian/fpga_code/fft/FFT_fft.ldc"

} out]} {
   runtime_log $out
   exit 1
}
