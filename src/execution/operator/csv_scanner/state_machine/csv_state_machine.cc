#include <goose/execution/operator/csv_scanner/csv_state_machine.h>
#include <goose/execution/operator/csv_scanner/sniffer/csv_sniffer.h>
#include <goose/utility/utf8proc_wrapper.h>
#include <goose/main/error_manager.h>
#include <goose/execution/operator/csv_scanner/csv_state_machine_cache.h>

namespace goose {

CSVStateMachine::CSVStateMachine(CSVReaderOptions &options_p, const CSVStateMachineOptions &state_machine_options_p,
                                 CSVStateMachineCache &csv_state_machine_cache)
    : transition_array(csv_state_machine_cache.Get(state_machine_options_p)),
      state_machine_options(state_machine_options_p), options(options_p) {
	dialect_options.state_machine_options = state_machine_options;
}

CSVStateMachine::CSVStateMachine(const StateMachine &transition_array_p, const CSVReaderOptions &options_p)
    : transition_array(transition_array_p), state_machine_options(options_p.dialect_options.state_machine_options),
      options(options_p), dialect_options(options.dialect_options) {
	dialect_options.state_machine_options = state_machine_options;
}

} // namespace goose
