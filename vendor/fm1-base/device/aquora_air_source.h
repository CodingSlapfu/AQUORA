/* SPDX-License-Identifier: MPL-2.0
 * AIR follows one advertiser, never an average of unrelated broadcasters. */
typedef struct {unsigned hash,last_seen,rejected;uint8_t address[6],random,valid;} aq_air_source;
static unsigned aq_air_source_take(aq_air_source *s,const uint8_t *p,unsigned n,unsigned now){
 if(n<8)return 0;
 unsigned random=(p[0]>>6)&1u,same=s->valid&&s->random==random;
 for(unsigned i=0;i<6;i++)if(s->address[i]!=p[i+2])same=0;
 if(s->valid&&!same&&now-s->last_seen<5000u){s->rejected++;return 0;}
 if(!same){
  s->hash=2166136261u;for(unsigned i=0;i<6;i++){s->address[i]=p[i+2];s->hash=(s->hash^p[i+2])*16777619u;}
  s->hash=(s->hash^random)*16777619u;if(!s->hash)s->hash=1;
  s->random=random;s->valid=1;
 }
 s->last_seen=now;return 1;
}
