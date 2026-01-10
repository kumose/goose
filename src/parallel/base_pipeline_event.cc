#include <goose/parallel/base_pipeline_event.h>

namespace goose {

BasePipelineEvent::BasePipelineEvent(shared_ptr<Pipeline> pipeline_p)
    : Event(pipeline_p->executor), pipeline(std::move(pipeline_p)) {
}

BasePipelineEvent::BasePipelineEvent(Pipeline &pipeline_p)
    : Event(pipeline_p.executor), pipeline(pipeline_p.shared_from_this()) {
}

} // namespace goose
