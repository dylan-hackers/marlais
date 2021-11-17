
#include <marlais/gc.h>

#include <gc.h>

#define W PRIuPTR

/* Internal functions */

static void marlais_gc_on_event (GC_EventType event);
static void marlais_gc_on_resize (GC_word new_size);

/* Exported functions */

void
marlais_initialize_gc (void)
{
  GC_init ();
  GC_set_on_collection_event(marlais_gc_on_event);
  GC_set_on_heap_resize(marlais_gc_on_resize);
  marlais_gc_report();
}

void
marlais_register_gc (void)
{
}

void
marlais_gc_collect (void)
{
  GC_gcollect();
}

void
marlais_gc_report (void)
{
  size_t sz;
  struct GC_prof_stats_s stat;

  sz = GC_get_prof_stats(&stat, sizeof(stat));
  if(sz != sizeof(stat)) {
    marlais_fatal("Error retrieving GC statistics");
  }

  fprintf(stderr, "\nGarbage collector statistics:\n\n");

  fprintf(stderr, "  Collection cycle:     %12"W"\n\n",
          stat.gc_no);

  fprintf(stderr, "  Heap memory:          %12"W" bytes\n",
          stat.heapsize_full);
  fprintf(stderr, "                        %12"W" used\n",
          stat.heapsize_full - stat.free_bytes_full);
  fprintf(stderr, "                        %12"W" free\n",
          stat.free_bytes_full);
  fprintf(stderr, "                        %12"W" fixed\n",
          stat.non_gc_bytes);
  fprintf(stderr, "                        %12"W" unmap\n\n",
          stat.unmapped_bytes);

  fprintf(stderr, "  Allocated before GC:  %12"W" bytes\n",
            stat.allocd_bytes_before_gc);
  fprintf(stderr, "  Allocated since GC:   %12"W" bytes\n",
          stat.bytes_allocd_since_gc);
  fprintf(stderr, "  Total allocations:    %12"W" bytes\n\n",
          stat.allocd_bytes_before_gc + stat.bytes_allocd_since_gc);

  fprintf(stderr, "  Reclaimed before GC:  %12"W" bytes\n",
            stat.reclaimed_bytes_before_gc);
  fprintf(stderr, "  Reclaimed since GC:   %12"W" bytes\n",
          stat.bytes_reclaimed_since_gc);
  fprintf(stderr, "  Total reclaimance:    %12"W" bytes\n\n",
          stat.reclaimed_bytes_before_gc + stat.bytes_reclaimed_since_gc);

  fflush(stderr);
}

static void
marlais_gc_on_event (GC_EventType event)
{
  bool flush = true;
  switch(event) {
  case GC_EVENT_START:
    marlais_gc_report();
    fprintf(stderr, "gc: collection starts\n");
    break;
  case GC_EVENT_MARK_START:
    fprintf(stderr, "gc: marking starts\n");
    break;
  case GC_EVENT_MARK_END:
    fprintf(stderr, "gc: marking ends\n");
    break;
  case GC_EVENT_RECLAIM_START:
    fprintf(stderr, "gc: reclaim starts\n");
    break;
  case GC_EVENT_RECLAIM_END:
    fprintf(stderr, "gc: reclaim ends\n");
    break;
  case GC_EVENT_PRE_STOP_WORLD:
    fprintf(stderr, "gc: stopping the world\n");
    break;
  case GC_EVENT_POST_STOP_WORLD:
    fprintf(stderr, "gc: the world has stopped\n");
    break;
  case GC_EVENT_PRE_START_WORLD:
    fprintf(stderr, "gc: starting the world\n");
    break;
  case GC_EVENT_POST_START_WORLD:
    fprintf(stderr, "gc: the world has started\n");
    break;
  default:
    flush = false;
    break;
  }
  if(flush) {
    fflush(stderr);
  }
}

static void
marlais_gc_on_resize (GC_word new_size)
{
  fprintf(stderr, "gc: heap resized to %"W" bytes\n", new_size);
}